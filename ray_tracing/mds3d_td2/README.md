d﻿

# TD2 - Maillage triangulaire
L'objectif de ce TD est dans un premier temps d'effectuer des lancer de rayon sur les triangles d'un maillage complet, pour ensuite mettre en œuvre une structure accélératrice afin d'optimiser le temps de calcul et d'affichage de la scene.
## 1. Bilan
### 1. 1 Calcul d'intersection
Il faut vérifier l'intersection du rayon partant de l'origine de la camera avec chaque triangle la technique de Möller-Trumbore qu'il faut implémenter dans *Mesh::IntersectFace*. Il s'agit de faire une translation d'un des sommets à l'origine du plan, pour ensuite aligner l'un des côtés du triangle sur l'un des axes du repère.
Dans un premier temps j'utilise la la normale de la face comme normale au point d'intersection dans le calcul de la technique. Afin d'obtenir cette normale, on effectue un produit vectorielle sur deux côtés du triangle :
$$\vec{N}=\vec{AC} X \vec{BC}$$
La normale calculé de cette façon donne le résultat suivant :

![](https://cdn.discordapp.com/attachments/673856583033159681/673856623319711744/e1.corsse2_tw.png "tw AC X BC")
On remarque que le maillage n'est pas vraiment lisse et que le traçage des triangles se voit beaucoup dans le résultat.
Il est possible de remédier à cela en utilisant les coordonnées barycentriques pour calculer la normale.
Pour ce faire on utilise les u et v calculés à partir de Möller-Trumbore afin de calculer la normale avec la formule suivante :
$$\vec{N}= Normal (A) * u + Normal(B) * v + Normal(C) * (1-u-v)$$
Où Normal(X) vaut la normale au point X
1-u-v car il faut que $\lambda_1+\lambda_2+\lambda_3=1$ où  $\lambda_1$ et $\lambda_2$ correspondent à u et v.

Avec les coordonnées barycentriques on obtient le resultat suivant pour le maillage "tw503.obj" :
![](https://cdn.discordapp.com/attachments/673856583033159681/673856644802936842/baricentre_tw503.png "tw 503 barycentre")

Et le resultat suivant pour le maillage "tw.obj":

![](https://cdn.discordapp.com/attachments/673856583033159681/673856633599950868/baricentre_tw.png "Barycentre tw.obj")

### 2.2 BVH
J'ai ensuite implémenté la structure accélératrice pour réduire le temps de calcul. Ce procédé se fait en 3 étapes.
#### 2.2.1 Build Node
Dans cette fonction , je calcule la boite englobante des faces comprises dans les indexes donnés en paramètre.
Si on atteint la condition d'arrêt *maxDepth* en paramètre ou si on atteint la limite de nombre de triangle *targetCellSize*, le noeud correspond à une feuille.
Pour les noeuds internes je cherche la plus grande dimension et je coupe au milieu de cette dimension.
J'appelle ensuite récursivement la fonction sur les fils gauche et droit de mon noeud actuel.
#### 2.2.2 Intersect Node
Je commence par le cas d'arrêt, si c'est une feuille j'appelle *intersectFace* sur chaque triangle de la boite acutelle.
Sinon je teste l'intersection du rayon avec les boites fils du noeud qu'on est en train de parcourir et je teste les cas suivants :
* Le rayon entre dans les deux boites et les boites se superposent
* Le rayon entre dans la première boite et les boites ne se superposent pas
* Le rayon dans la première boite et la deuxième boite non superposés mais n'a pas trouvé d'intersection dans la première
* Le rayon touche juste la deuxième boite et les boites ne se superposent pas

#### 2.2.3 Intersect
J'appelle *intersectNode* s'il y a intersection avec la boite englobante de la racine de l'arbre.

Une fois ces trois fonctions implémentés, j'ai effectué les tests sur les machines de la salle 005 du cremi :
* Cpu : E3-1240 v5 @ 3.50GHz, 16Go Ram
* GPU : GTX 960 4 Go
Et voici les résultats avec le temps et le nombre de triangles testés :

![](https://cdn.discordapp.com/attachments/673856583033159681/674157454942011403/unknown.png "tw.obj BVH")

![](https://cdn.discordapp.com/attachments/673856583033159681/674157946568966144/unknown.png "Killeroo BVH")

![](https://cdn.discordapp.com/attachments/673856583033159681/674158327286202378/unknown.png "KillerooGlass BVH")

## 2. Bugs résolus

J'ai écrit le premier cas de mon *intersectNode* en mettant directement dans la valeur de retour un OU logique sur l'appel recursif. Ce qui fait que mon algo faisait uniquement les fils gauches ce qui donne les bugs ci dessous :
![](https://cdn.discordapp.com/attachments/673856583033159681/673856667296989206/bug_kr_oubli_test_fils_droite_quand_semboitent.png.png "Killeroo bug")

![](https://cdn.discordapp.com/attachments/673856583033159681/673856676687773707/bug_tw_oubli_test_fils_droite_quand_semboitent.png.png "tw bug")
## 3. Bugs non résolus et améliorations possible

J'ai réussi à implémenter toutes les fonctions néanmoins changer la méthode de coupe pour les boites en utilisant la formule de coût pour tracer les rayons pourrait être une solution pour encore plus optimiser les algorithmes.
