# Conception de l'Architecture ECS (Entity-Component-System) {#architecture_ecs}

## 1. Pourquoi ECS
Le choix d'un système Entity-Component-System repose principalement sur le fait qu'il est pensé pour s'orienter autour de la donnée (DOD - Data-Oriented Design). C'est-à-dire qu'il est construit de sorte à ce que le matériel puisse accéder aux données de manière optimisée en évitant au maximum les défauts de cache (cache miss). Contrairement à un système OOP classique, on ne travaille plus sur des objets/classes qui contiennent chacun leurs données et les éparpillent là où leurs jambes les mènent. L'OOP impose un surcoût en performances inhérent aux concepts de haut niveau comme l'héritage ou les fonctions virtuelles (vtable, indirection de pointeurs et les cache misses liés à ceux-ci).

C'est cette approche DOD qui permet au moteur de boucler rapidement sur une quantité de données importante et de les mettre à jour plus vite que son ombre. C'est un besoin vital pour un moteur de jeu destiné à gérer un nombre d'entités imposant.

Un autre avantage crucial est la modularité. La séparation des systèmes (la logique) et des composants (les données) permet l'ajout ou le retrait de composants de manière dynamique et les rend hautement réutilisables.

Dernier point et pas des moindres : c'est quand même un système vachement cool. Pygame peut aller se rhabiller.

## 2. Les Entités (Entity)
Contrairement à ce que l'on pourrait attendre, une entité n'est pas une classe objet qui contient des données ou une logique métier. Ce n'est qu'un wrapper (une enveloppe ou "poignée") qui, comme son nom le suggère, va enrober une autre classe : le registre. Son seul but est de fournir à l'utilisateur une syntaxe beaucoup plus agréable. Celle-ci permet de fourrer l'entité de composants et, par la même occasion, de nous régaler les mirettes.

### Structure
Il faut voir l'entité comme un simple identifiant. En réalité, elle n'est composée que de trois éléments :
- `EntityId` (`uint32_t`) : un identifiant
- `EntityVersion` (`uint32_t`) : une version
- `Registry*` : un pointeur vers le registre qui la possède

### Versioning 
Il est important de comprendre que l'ID et la version que l'entité stocke ne font pas foi. Ce n'est qu'un "ticket" émis par le registre lors de la création. Ces identifiants sont fixés dans le marbre dans l'objet `Entity`.

De l'autre côté du guichet, c'est le registre qui garde les données réelles à jour. Il faut le voir comme le régisseur : sa vérité fait foi. Quand on crée ou détruit une entité, le registre modifie la version dans ses propres bases de données selon ces règles :
- Chaque ID est associé à sa version, qui commence à 0 lorsque l'ID n'a jamais été utilisé.
- Détruire une entité incrémente sa version côté registre et rend l'ID disponible pour une nouvelle entité (la version passe à un nombre impair).
- Recréer une entité à partir d'un ID recyclé incrémente de nouveau sa version (elle redevient paire).
- Une entité morte a donc un numéro de version impair, et une entité vivante un numéro pair.

La version sert ainsi à vérifier qu'une entité est toujours valide. Comme chaque action effectuée n'est en réalité qu'une requête envoyée au registre, on a la sécurité de ne jamais accéder à un recoin mémoire qu'on préférerait ne pas découvrir.

## 3. Le Stockage : Le Sparse Set
Le Sparse Set est le cœur du système ECS. Son rôle est de stocker des composants et de les associer à un identifiant afin d'y accéder de manière quasi instantanée en $O(1)$. De plus, il les stocke de manière à ce qu'un parcours séquentiel de ces données soit optimal pour le processeur.

Il offre des performances CPU inégalées, en contrepartie d'un stockage mémoire volontairement peu économe. Cette structure a été choisie pour éviter les problèmes de fragmentation de l'OOP et garantir une efficacité maximale.

### Le problème de l'OOP 
Dans l'approche classique, les données et les logiques sont fragmentées en mémoire. Il faut y accéder par des pointeurs et des vtables lors de l'utilisation de l'héritage. Cela ralentit fortement la lecture et l'écriture, qui arrivent des milliers de fois par frame et se doivent d'être les plus rapides possible.

### Fonctionnement du SparseSet\<T>
Pour chaque type de donnée (Composant), le moteur instancie un Sparse Set indépendant.

On stocke les données pures dans le vecteur `denseData<T>`. Elles y sont parfaitement alignées en mémoire (d'où le terme "dense") afin de maximiser l'utilisation du cache CPU. Cela permet de parcourir la liste des données à la vitesse de la lumière.

Pour pouvoir lier une Entité à sa donnée en $O(1)$, on utilise un vecteur d'index `sparse<size_t>`. Les positions (index) de ce vecteur représentent les IDs des entités. La case contient l'index du tableau `denseData` où se trouve la vraie donnée. Si l'entité ne possède pas ce composant, on y place une valeur nulle arbitraire.

Enfin, pour retrouver à quelle entité un composant dense appartient (faire le chemin inverse), on utilise le tableau `denseIds<EntityId>`. Il est vital pour deux raisons :
- Lors du parcours d'une "View", il permet de savoir instantanément quelles entités possèdent le composant.
- Lors de la suppression d'un composant, on utilise la technique du Swap and Pop (remplacer l'élément supprimé par le dernier élément du tableau pour boucher le trou). Cette action mélange les positions dans le tableau dense. Sans `denseIds`, il serait impossible de savoir à quelle entité appartenait ce dernier élément déplacé pour mettre à jour le tableau `sparse`.

### Compromis
Le compromis de cette structure est que le tableau `sparse` contient des trous. Si sur mille entités, une seule possède le composant, le tableau `sparse` fera quand même mille cases de long pour une seule information utile. On accepte de "gaspiller" quelques kilo-octets de mémoire (pour stocker des entiers vides) au profit d'une vitesse d'exécution CPU absolue.

## 4. Le Registre (Registry)
Le registre fait office de Ministère de la Vérité. C'est lui qui centralise les données, sait où en sont les couples ID/Version des entités, et manipule les mémoires. On passe toujours par le registre pour accéder à un composant, le modifier ou en ajouter.

### Gestion centralisée et Sécurité
Il génère les IDs, tant ceux des entités que ceux des types de composants. Cette génération se fait à la volée en assurant que :
- Les IDs d'entités générés sont locaux (propres à un registre spécifique).
- Les IDs des types de composants sont globaux (statiques pour tous les registres du programme).

En tant qu'intermédiaire obligatoire, il cache non seulement la gestion interne de l'allocation des ressources, mais s'assure aussi que tout est fait de manière sécurisée. Cela garantit à l'utilisateur qu'il n'y a qu'une seule vérité, et que le seul habilité à la délivrer est le Registre.

### Indexation des types de composants en O(1)
Pour associer un type C++ `T` à son `SparseSet<T>` sans passer par une `std::unordered_map` (lente à cause du hachage), le registre utilise une fonction template statique. À la compilation/premier accès, chaque type `T` reçoit un identifiant entier unique (`0, 1, 2...`). Le registre stocke ses SparseSets dans un vecteur contigu et y accède directement par index. L'accès au tableau d'un composant est ainsi instantané en $O(1)$.


## 5. Les Vues (Views) : Lecture et écriture de masse
La `View<Components...>` est l'outil de requête délivré par le Registre. Elle permet d'itérer à toute vitesse sur l'ensemble des entités possédant une combinaison spécifique de composants (par exemple `Position` et `Velocity`). Elle est la pièce maîtresse du moteur : toutes les autres structures sont conçues pour lui permettre d'exécuter ses requêtes le plus rapidement possible. C'est un peu comme un livreur immigré et en plus elle ne se plaint pas.

### La Loi du Plus Petit
Lorsqu'un système demande au Registre de croiser plusieurs composants, la View applique la "Loi du plus petit" pour maximiser les performances :
1. Elle identifie quel `SparseSet` parmi ceux demandés contient **le moins d'éléments** dans son tableau dense.
2. Elle effectue sa boucle `for` uniquement sur le tableau d'entités de ce plus petit jeu de données.
3. À chaque itération, elle interroge le tableau `sparse` des autres composants requis pour vérifier leur présence en $O(1)$.

Si 10 000 entités ont une `Position` mais que seules 50 ont une `Velocity`, la View ne fera que 50 tours de boucle au lieu de 10 000. Les 9 950 autres entités ne seront jamais lues par le processeur.

### Pourquoi Diable le CRTP (pour les itérateurs)
La View propose deux manières de parcourir les données :
- La méthode `each()` : prend une fonction lambda en paramètre pour un traitement direct.
- Les itérateurs C++ (compatible boucle `for (auto [id, pos, vel] : view)`) : plus lisibles et idiomatiques.

Afin d'éviter la duplication de code entre les différents types d'itérateurs (itérer sur les IDs seuls, les composants seuls, ou les entités), le moteur utilise le pattern **CRTP** (*Curiously Recurring Template Pattern*). Ce polymorphisme statique permet de partager la logique complexe d'avancement dans les tableaux tout en garantissant des performances maximales à l'exécution : zéro vtable, zéro indirection de pointeur et inlining total par le compilateur.

## 6. Alternatives considérées
Concevoir une architecture, c'est avant tout faire des choix et accepter des compromis. Voici les approches techniques que nous avons étudiées puis délibérément écartées lors de la conception de ce moteur, et pourquoi.

(Ecrit par Gemini, basé sur des conversations intimes)

### Les Archétypes (Façon Unity DOTS / Unreal Mass)
Dans un modèle par Archétypes, le moteur ne stocke pas un tableau par type de composant, mais regroupe physiquement les entités selon leur "signature" exacte. Par exemple, toutes les entités possédant la combinaison `[Position + Sprite]` sont stockées ensemble dans un gros bloc mémoire où les données sont parfaitement entrelacées. L'espace mémoire est optimisé à 100 % (zéro trou).

Pourquoi l'avoir rejeté ? 
- Complexité d'implémentation : C'est une usine à gaz à coder pour un premier moteur.
- Le coût des mutations : Si, en plein jeu, on ajoute un composant `Poison` à une entité, sa "signature" change. Le moteur doit alors copier/coller physiquement toutes les données de cette entité d'un bloc Archétype A vers un bloc Archétype B. Si le jeu ajoute et retire des composants très fréquemment, ce déplacement de mémoire constant détruit les performances. Le Sparse Set offre une flexibilité de mutation immédiate et sans coût.

### L'OOP Orienté Composition (Le modèle "Actor-Component" classique)
C'est le modèle historique de Unity (avec les `GameObjects` et `MonoBehaviours`) ou d'Unreal Engine classique (avec les `AActor` et `UActorComponent`). L'entité est un véritable objet instancié en mémoire qui maintient une liste de pointeurs vers ses composants.

Pourquoi l'avoir rejeté ? 
- Le pire ennemi du processeur : Les données sont instanciées un peu partout sur le heap (tas) au gré des allocations. Le CPU passe son temps à bondir d'une adresse mémoire à une autre (provoquant d'innombrables cache misses).
- Le surcoût virtuel : Utiliser des classes de base et de l'héritage impose la création de VTables (tableaux de méthodes virtuelles). Ce surcoût à l'exécution est incompatible avec notre volonté de traiter des dizaines de milliers d'entités par frame.

### La `std::unordered_map` pour indexer les composants
Pour que le Registre puisse retrouver le bon `SparseSet` à partir d'un type de composant demandé par l'utilisateur (ex: `registry.get<Position>(entity)`), l'approche la plus intuitive en C++ était d'utiliser un dictionnaire avec le typeid comme clé : `std::unordered_map<std::type_index, ISparseSet*>`.

Pourquoi l'avoir rejeté ? 
- Lenteur d'accès : Bien que ce soit logiquement correct, une table de hachage (hashmap) impose de calculer le hash du type et de gérer des collisions potentielles à chaque accès. Appeler cette fonction des milliers de fois par frame créait un goulot d'étranglement majeur.

La solution retenue : 
- Nous l'avons remplacée par la "magie" des templates C++ (un générateur d'identifiants statique). Chaque type de composant reçoit un ID unique continu (`0, 1, 2...`) à la compilation/premier appel. Le registre n'a plus qu'à faire une simple lecture d'index dans un vecteur : `components[id]`. C'est un accès mathématiquement pur en $O(1)$, sans aucun calcul de hash.

### Les "Systèmes Lambdas" dans les Composants
Une approche étudiée fut de stocker des fonctions anonymes (`std::function` ou lambdas) directement à l'intérieur des composants pour simuler des fonctions `Update()` locales sans utiliser d'héritage virtuel.

Pourquoi l'avoir rejeté ? 
- Pureté de l'architecture : Cela violait la philosophie fondamentale du Data-Oriented Design. Un composant doit rester une structure de donnée pure et muette (POD - Plain Old Data). La logique doit impérativement rester séparée, globalisée et traitée par lots dans les Systèmes pour garantir les performances et la modularité.