#include "ECS/View.hpp"
#include <algorithm>
#include <type_traits>

// Included at the end of Registry.hpp to solve a circular dependencie.

namespace nut
{
    template<typename... ComponentTypes>
    View<ComponentTypes...>::View(Registry* r, internal::SparseSet<ComponentTypes>*... dataPack)
    : registry(r), data(dataPack...)
    {
        std::array<const std::vector<EntityId>*, sizeof...(ComponentTypes)> allEntities = { 
            &(dataPack->getDenseId())... 
        };

        minEntities = *std::min_element(allEntities.begin(), allEntities.end(), 
            [](const std::vector<EntityId>* a, const std::vector<EntityId>* b) 
            {
                return a->size() < b->size();
            }
        );
    }

    template<typename... ComponentTypes>
    template<typename F> 
    void View<ComponentTypes...>::each(F func)
    {
        for (EntityId id : *minEntities)
            if (contains(id))
            {
                if constexpr (std::is_invocable_v<F, nut::EntityId, ComponentTypes&...>) 
                    func(id, std::get<internal::SparseSet<ComponentTypes>*>(data)->get(id)...);
                else if constexpr (std::is_invocable_v<F, ComponentTypes&...>) 
                    func(std::get<internal::SparseSet<ComponentTypes>*>(data)->get(id)...);
                else 
                    // Dependent false to defer static_assert evaluation until template instantiation
                    static_assert(internal::AlwaysFalse<F>::value, "View::each called with invalid arguments");
            }
    }

    // to avoid cyclic inclusion with registry.hpp
    template<typename... ComponentTypes>
    Entity View<ComponentTypes...>::EntityIterator::operator*() const 
    {
        EntityId currentId = (*this->view->minEntities)[this->index];
        return Entity(currentId, this->view->registry->getVersion(currentId), this->view->registry); 
    }
}