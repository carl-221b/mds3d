
# TD1 - Recursive ray tracing



Dans ce TD nous allons implémenter la synthétisation d'image grâce à la méthode de ray tracing. Nous allons commencer par un lancer de rayon qui retranscrit juste la couleur du pixel sur le modèle 3D. On va ensuite raffiner le rendu en prenant en compte différent paramètres comme l'éclairage, la texture de l'objet, les ombres et les reflets.
Toutes les images sont en URL absolu dans un serveur Discord que je me suis créé, mais j'ai également joint chaque image dans le dossier *resultats* fourni avec mon archive.



## 1. Bilan

### 1. Rayons primaires
#### 1.1 Génération de rayons

Dans la fonction *render* du fichier "main.cpp" je fait un parcours de la grille de pixel afin de déterminer la direction du rayon pour chaque pixel. L'origine correspond juste à la position de la camera.

Il y a le vecteur camF qui représente la direction de la camera vers la grille de pixel, camX et camY représentent  les axes sur l'image.
Le vecteur représentant la direction du rayon s'obtient avec la déviation de camF avec camX et camY selon un certain coefficient. On aura alors la formule de la forme :
$$ d= camF + \alpha * camX + \beta * camY $$
Où $\alpha$ et $\beta$ sont les coefficients d'orientation de déplacement.
Pour un pixel (i,j) dans la grille :

*  $\alpha = 2* \frac{i}{vpWidth} - 0.5$
* $\beta = 2* \frac{j}{vpHeight} - 0.5$

Pour recentrer les deux coefficients  $\frac{i}{vpWidth} et \frac{j}{vpHeight}$ dans l'intervalle [-1,1] je soustrait par 0.5 puis je multiplie les deux coefficients  2 afin de retrouver finalement avec la formule suivante pour chaque pixel à la position (i,j) dans la grille :

$$ d= camF + 2* \frac{i}{vpWidth} - 0.5* camX + 2* \frac{j}{vpHeightih} - 0.5 * camY $$

Il est alors possible d'instancier notre rayon avec les deux paramètres position de la camera (son point d'origine) et le vecteur d qu'on vient de calculer (sa direction).

J'ai ensuite implémenté la fonction *intersect* dans le fichier "scene.cpp" où pour chaque objet de la scène je vérifie s'il y a une intersection entre l'objet et le rayon en paramètre.
S'il y a une intersection pour un rayon donné on met à jour la distance t parcourue de l'origine du rayon vers l'intersection ainsi que la forme de l'objet intersecté dans une variable de type *Hit*. S'il y a plusieurs intersections on garde l'objet le plus proche de la camera (avec la distance parcourue la plus faible).

#### 1.2 Sphères

Pour afficher les sphères il fallait trouver le point d'intersection entre l'objet et le rayon le plus proche. Afin de déterminer ce point il faut résoudre l'équation :
$$||p-c||^{2}-r^{2}=0$$
Une fois développé j'obtient la formule de second degré de la forme $at^{2}+bt+c=0$ suivante :
$$t^{2} d^{2} + t (2od-2dc)+(-2oc+o^{2}+c^{2}-r^{2})=0$$

En fonction de la valeur du discriminant qui en résulte on met à jour le temps *t* qu'il faut pour trouver l'intersection ainsi que le matériau trouvé.

#### 1.3 Première image
Dans un premier temps je retourne juste la couleur ambiante de l'objet intersecté dans l'intégrateur "flat.cpp" qui sera récupéré dans le main afin d'obtenir le résultat suivant sur "TroisSpheres.scn" :

![](https://cdn.discordapp.com/attachments/670300931681943553/670301015907631104/1.2.troisSpheres.png "Trois Sphère Flat")


#### 1.4 Plans
Pour le plan j'ai cherché une expression de t à partir de la formule (p-a)*n=0 où p= dt+o , a est un point à sa surface et n sa normale.
En développant, j'ai retrouvé l'expression de t suivante:
$$t=\frac{n(-o+a)}{dn}$$
La valeur de t permet alors de connaitre la position du rayon face au plan. Dans mon cas j'ai trouvé pertinent gérer le cas où t est indéfini (dn<0) et t est positif, c'est dans le deuxième cas que l'intersection est devant la camera.
Je met alors le t calculé dans la variable *hit* et je met également à jour les autres informations pour retrouver le resultat suivant sur "petanque.scn" :

 ![](https://cdn.discordapp.com/attachments/670300931681943553/670301040868065291/1.4.petanque.png "Pétanque Flat")

### 2. Les Sphères
#### 2.1 Normales

J'ai ajouté le calcul de la normale dans le fichier "sphère.cpp" qui peut être calculé avec : n = p-c. Où n est la normale, p le point d'intersection entre le rayon et la sphère, et c le centre de la sphère. Il faut ensuite stocker cette normale dans le champ correspondant dans *hit* puis dans l'intégrateur créer une couleur avec la valeur absolue des trois composantes du vecteur pour avoir l'affichage demandé pour "TroisSphere.scn" :

![](https://cdn.discordapp.com/attachments/670300931681943553/670301071586885671/2.1.troisSpheres.png "TroisSpheres Normals")

#### 2.2 Calcul de l'éclairage

J'ai commencé par implémenter les méthodes qui renvoient la direction et l'intensité de la lumière. Ensuite la méthode qui renvoie la brdf de phong dans le fichier "phong.cpp". Dans cette méthode il faut renvoyer la somme de la couleur spéculaire et diffuse du matériau. La couleur diffuse est contenue dans les propriétés du matériau:
$$ \rho_{d}=m_{d}$$

$m_d$ étant la couleur diffuse du matériau.

 La couleur spéculaire s'obtient avec la formule suivante :
$$ \rho_ {s}=m_{s} (max(r.v,0))^{s}$$

$m_{s}$ étant la couleur spéculaire, r la lumière réfléchie par rapport à la normale, v le vecteur directeur du point de vue et s la brillance de l'objet.
$$\rho = \rho_{s}+\rho_{d} $$
Une fois ces fonctions implémentés il est alors possible dans l'intégrateur "direct.cpp" d'itérer sur toutes les lumières de la scène  afin de calculer la couleur au point d'intersection entre le rayon partant de la camera et l'objet intersecté. Cette couleur dépend de la brdf, l'intensité de la lumière et le cosinus de l'angle entre la normale au point d'intersection et la direction de la lumière :
$$ CouleurResultat= CouleurResultat + brdf * max(<ldir.n>,0) * lightIntensity $$

Je cumule ces couleurs dans une variable qui a été défini avant l'itération pour ensuite la renvoyer dans la fonction  de l'integrateur.

Cet étape permet alors d'avoir les resultats attendus pour "phong.scn" et "TroisSpheres.scn" :

![](https://cdn.discordapp.com/attachments/670300931681943553/670301102205304846/2.2.troisSpheres.png "Trois Sphere Direct")
![](https://cdn.discordapp.com/attachments/670300931681943553/670301092453679115/2.2.phong.png "Phong direct")


### 3. Rayons secondaires
#### 3.1 Ombres portés
Afin d'identifier les ombres portés, il faut tester au point d'intersection du rayon partant de la camera avec l'objet si une source de lumière est visible.
Je lance alors un nouveau rayon à partir de ce point vers chaque source de lumière afin de tester si la lumière est visible en ce point.

Si ce nouveau rayon trouve un point d'intersection c'est qu'il y a un objet dans la même direction que la lumière. Il faut alors verifier que ce point est bien devant la lumière, si c'est le cas c'est que cet objet fait de l'ombre au point d'origine du nouveau rayon. Sinon on renvoie la couleur calculée plus tôt dans la partie 2.2.

En ajoutant cette partie dans l'integrateur "direct.cpp" on obtient les bons ombrages pour "TroisSpheres.scn" :

![](https://cdn.discordapp.com/attachments/670300931681943553/670301209176965150/3.1.troisSpheres.png "TroisSphere Ombrage")

#### 3.2 Rebonds multiples

Pour gérer les surfaces réfléchissantes, on relance un rayon à partir du point d'intersection vers la direction réfléchie par rapport à la normale. On ajoute à la couleur résultat un appel récursif à Li la fonction de l'integrateur avec le rayon réfléchi en paramètre pondéré par la reflectivité du matériau et le cosinus de l'angle entre le rayon réfléchie et la normale :

 $$ CouleurResultat = CouleurResultat + Li(scene,rayonReflechi) * reflectivité * <rayonReflechi.normale> $$

On incrémente le niveau de recursion et si le niveau de recursion atteint le niveau maximum de recursion autorisé (contenu dans le matériau) on s'arrête et on renvoie la couleur.
Cette couleur renvoyé aura alors pris en compte toutes les reflections du matériau et les resultats sont bien ceux qui sont attendus :

![](https://cdn.discordapp.com/attachments/670300931681943553/670360391389413386/3.2.troisSpheres.png "TroisSphere Reflection")

![](https://cdn.discordapp.com/attachments/670300931681943553/670360605429071902/3.2.phong.png "Phong Reflection")

![](https://cdn.discordapp.com/attachments/670300931681943553/670360595895287809/3.2.petanque.png "Petanque Reflection")

### 4. Bonus
#### 4.1 Textures
La texture d'un objet sphérique peut s'exprimer à partir d'un vecteur $\vec{uv}$ dont u et v se calculent par :

$$ u= 0.5+ \frac{arctan2(normale.x,normale.y)}{2\pi} $$
$$ v= 0.5 + \frac{asin(normale.z/rayon)}{\pi}$$

Je stocke ce vecteur dans la variable hit afin de le reutiliser dans brdf pour prendre en compte la texture de l'image en la donnant en paramètre à la methode diffusecolor.
Pour tester les coordonnées de texture je les écrit sous forme de couleur dans un integrateur test "textcoords.cpp" ce qui donne le resultat suivant :

![]( "Texcoords test")

Le test donne bien ce qui est attendu.Pour les plans les paramètres du vecteur u et v sont les coordonnées x et y au point d'intersection entre le plan et le rayon.
Je peux maintenant appeler uv dans la méthode diffusecolor dans brdf, voici quelques resultats des images une fois la texture appliquée:

![](https://cdn.discordapp.com/attachments/670300931681943553/670379382149349376/4.1.troisSpheres.png "Trois Sphere texture")

![](https://cdn.discordapp.com/attachments/670300931681943553/670371875280650245/4.1.petanque.png "Petanque texture")

![](https://cdn.discordapp.com/attachments/670300931681943553/670371911422836781/4.1.deuxPlans.png "Deux Plans texture")

![](https://cdn.discordapp.com/attachments/670300931681943553/670372014715830282/4.1.tasSpheres.png "Tas Sphere texture")

![](https://cdn.discordapp.com/attachments/670300931681943553/670372125038870528/4.1.terre-lune.png "Terre Lune")

#### 4.2 Réfractions
Dans la condition de recursion je teste si le materiau et refractante si c'est le cas je calcule le rapport entre les deux milieux d'incidence.
Je calcule ensuite le vecteur du rayon refracté en prenant compte du signe du cosinus de l'angle de refraction afin de savoir si on passe d'un milieu au matériau ou du matériau vers un autre milieu.
L'appel récursif fera que les deux cas seront prise en compte, j'ajoute ensuite dans la Couleur Resultat l'appel recursif à Li avec le rayon réfracté en paramètre pondéré par la transmissivité du matériau.


## 2. Bugs résolus
Une des difficultés de ce TD a été la compréhension de la formule de prise en compte pour la texture. En appliquant exactement la formule sur la page Wikipedia UV Mapping je me retrouvais avec le resultat suivant :

![](https://cdn.discordapp.com/attachments/670300931681943553/670379518296457236/bug_atan_z_x_sin_y.png "Tecoords xz y")

Sur la photo on remarque que les couleurs ont été décalés, je me suis donc dit que c'était un problème de coordonées. En interchangeant les coordonnées de la normale dans atan et asin dans le calcul de u et v j'ai pu obtenir un resultat se rapprochant à ce qui est attendu.
C'était donc pas arctan2(dz,dx) asin(dy) mais arctan2(x,y) et arcsin(z).
Je me retrouvait donc avec le resultat ci-dessous, le côté bas des sphères n'est pas exactement de la même couleur que ce qui est attendu.

![](https://cdn.discordapp.com/attachments/670300931681943553/670379534821883934/bug_sans_radius.png "Texcoords / radius")

Je me suis alors dit que pour recentrer dans l'intervalle [0,1] il fallait diviser par le rayon. J'ai essayé sur ma formule et ça m'a donné le bon resultat (cf 1.4.1).

## 3. Bugs non résolus

Sur le terminal j'ai des messages disant "invalid radiance value"  ce qui fait que les resultats de la réfraction ne sont pas comme ce qui est attendu. Je suppose que j'ai mal orienté l'un des vecteurs en gérant le cas de la réfraction.
Voici le resultat de ce que j'ai essayé d'implémenter:

 ![](https://cdn.discordapp.com/attachments/670300931681943553/670379549426450432/bug_deuxSpheres.png "Refraction bug")
