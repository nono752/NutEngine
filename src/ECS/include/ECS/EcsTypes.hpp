#ifndef ECS_TYPES_H
#define ECS_TYPES_H

/**
 * @file EcsTypes.hpp
 * @brief Définition des types de base et des constantes globales pour le module ECS.
 */

#include <cstdint>
#include <limits>
#include <type_traits>

namespace nut 
{
    /**
     * @addtogroup ECS_Module
     * @{
     */

    /** @brief Type représentant l'identifiant unique généré pour un type de composant. */
    using ComponentId = uint32_t;

    /** @brief Type représentant l'identifiant brut (l'index en mémoire) d'une entité. */
    using EntityId = uint32_t;

    /** @brief Type représentant la génération/version d'une entité (utilisé pour sécuriser les recyclages). */
    using EntityVersion = uint32_t;

    /** @brief Constante représentant un identifiant d'entité invalide (valeur maximale du uint32_t). */
    constexpr EntityId NULL_ENTITY_ID = std::numeric_limits<EntityId>::max();

    /** @brief Constante représentant une version d'entité invalide ou nulle. */
    constexpr EntityVersion NULL_ENTITY_VERSION = std::numeric_limits<EntityVersion>::max();

    /** @} */ // Fin du groupe ECS_Module


    /**
     * @internal
     * @namespace nut::internal
     * @brief Espace de noms réservé à la mécanique interne du moteur.
     * @warning Les éléments de ce namespace ne font pas partie de l'API publique et peuvent 
     * être modifiés ou supprimés à tout moment sans préavis.
     */
    namespace internal 
    {
        /**
         * @internal
         * @brief Structure utilitaire pour forcer l'évaluation différée d'un static_assert.
         * * En C++, un `static_assert(false)` dans un template bloque la compilation même si 
         * le template n'est pas utilisé. En utilisant `static_assert(AlwaysFalse<T>::value)`, 
         * l'erreur ne se déclenche que si le développeur instancie réellement cette branche (utile 
         * pour interdire explicitement certaines conversions ou types).
         * * @tparam T Le type intercepté (non utilisé dans la structure, sert juste de déclencheur).
         */
        template<typename T>
        struct AlwaysFalse : std::false_type {};
    }
}

#endif