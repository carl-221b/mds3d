
# TD7 - Normal Mapping
## Bilan
### 1. Normal Mapping sur un plan

* Lumière dans l'espace objet :
Tout l'objet est éclairé alors que le comportement normal serait qu'une seule partie de l'objet est éclairé et la lumière reste à un endroit fixe sur l'objet.

* Lumière dans l'espace vue : 
Le comportement de la lumière n'est pas normale, on a l'impression qu'elle éclaire une bonne moitié de l'objet et pas l'autre indépendamment de la position de la lumière.

Ceci est dû au fait que le plan est placé de façon à ce que les normales contenues dans la carte de normales correspondent à l'espace objet, or ce n'est pas le cas, la carte de normales est en réalité exprimée dans le repère tangent à la surface de l'objet.

Voici les résultats que j'ai eu pour le plan :
![](https://cdn.discordapp.com/attachments/691636286225776701/691637522924699678/diffuse_normal.png "Normal mapping plan")
![](https://cdn.discordapp.com/attachments/691636286225776701/691637521003708466/diffuse.png "diffuse")

### 2. Calcul du repère tangent

Dans cette partie j'ai calculé la tangente et bitangente par face puis j'ai complété compute Normals en calculant la tangente et bitangente comme expliqué dans les slides de cours en utilisant la méthode matrice inverse.

### 3. Normal mapping sur une sphère

Pour cette partie je crée la tbnvMatrix avec la normale, tangente et bitangente qu'on a calculé dans compute normals.
Dans l'appel à la fonction blinn je multplie par tbnvMatrix le vecteur de vue et le vecteur lumière.
Ce qui donne les resultats suivants :
![](https://cdn.discordapp.com/attachments/691636286225776701/691637515458969600/terre_diffus.png "Diffuse terre")

![](https://cdn.discordapp.com/attachments/691636286225776701/691637513609281567/terre_color.png "terre color")

## Quelques bugs rencontrés 

Je me suis trompé sur les adresses en envoyant la tangent et la bitangente au vertex shader ce qui fait un effet étirée sur la planète. En faisant attention aux adresses j'ai pu avoir la terre comme il faut.

![](https://cdn.discordapp.com/attachments/691636286225776701/691637516557615204/bug_etiree.png "bug adresse")

J'ai oublié d'utiliser *earth2.obj* comme modèle ce qui a fait le petit bug suivant :

![](https://cdn.discordapp.com/attachments/691636286225776701/691637512678015026/joshu.png "bug joshu")
