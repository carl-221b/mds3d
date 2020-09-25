# TD4 -  Transformations
Toutes les photos données en lien dans ce Markdown sont également dans l'archive  dans le dossier " resultats " 
## Entrainement rotaltion et translation chaise
Rotation sur l'axe y de la chaise avec redimensionnement et translation des chaises. ![](https://cdn.discordapp.com/attachments/681520398927659008/681558458427899952/chair_and_small_chairs.png "chair translation rotation")

## Camera 
Implémentation de la fonction *Camera::lookat* qui est appelé dans viewer.cpp afin de choisir la position de la camera, le target sur la scene et l'inclinaison de la camera.
Implémentation de *Camera::rotateAroundTarget* pour la classe trackball afin de faire tourner la camera autour de la scene.

##   Système solaire

Création des matrices de translation et de redimension pour créer le soleil, la terre et la lune à partir de sphere.off.
J'initialise les matrices de la terre et de la lune dans l'initialisation de mesh.cpp pour les placer dans la scene avec les translation et les inclinaisons qu'il faut 
* inclinaison de 23.44° par rapport au soleil pour la terre (en axe Z)
* inclinaison de 5.14° par rapport à la terre pour la lune (en axe Z)
![](https://cdn.discordapp.com/attachments/681520398927659008/681558450928615443/planete_normals.png "Systeme solaire")
J'ai également crée deux shaders *wire.vert* et *wire.frag* pour afficher en fil de fer afin de voir la rotation des planètes. Vous pouvez l'activer et désactiver avec la touche 'L'

![](https://cdn.discordapp.com/attachments/681520398927659008/681565024493633546/wire.png "Wire")
# TD5 - Eclairage et normales

##  Calcul de l'éclairage

Implémentation de la fonction blinn dans un premier temps dans *blinn.vert*pour ensuite le déplacer dans *blinn.frag* afin d'avoir des gains de reflet.
J'envoie les matrices de vue, projection , objet et à l'aide de glUniform ainsi que normal_mat.
L'intensité de la lumière est codé en dure dans *blinn.frag* afin de ne pas avoir à l'envoyer au shader pour chaque planète.
Voici les différents composantes de la fonction blinn comme spécifié dans le sujet avec une couleur vec3(0,0,1).

Couleur spéculaire : 
![](https://cdn.discordapp.com/attachments/681520398927659008/681558449460477953/specular.png "Speculaire")

Couleur ambiant :

![](https://cdn.discordapp.com/attachments/681520398927659008/681558459702968364/ambiant_color.png "Ambiant")

Couleur diffuse :

![](https://cdn.discordapp.com/attachments/681520398927659008/681558457345769555/diffuse.png "Diffuse")

Le tout avec Blinn :

![](https://cdn.discordapp.com/attachments/681520398927659008/681558452799406112/modele_phong.png "Blinn")

Monkey2.obj avec blinn : 

![](https://cdn.discordapp.com/attachments/681520398927659008/681777685424504859/suzanne.png " Suzanne blinn")

Voici maintenant quelques exemples du système solaire avec la lumière :

Avec la couleur bleue vec3(0,0,1) :
![](https://cdn.discordapp.com/attachments/681520398927659008/681558446881112131/terre_lune_soleil.png "Systeme bleue")




## Compute Normals

Implémentation de la fonction *Mesh::ComputeNormals*selon l'algorithme proposé par le sujet : 

1.  Mettre à zéro les normales des sommets
2.  Pour chaque face, calculer sa normale (sans la normaliser) et l'ajouter à la normales des sommets de la face
3.  Normaliser les normales des sommets

Il faut ensuite l'appeler avant _mesh.initVba() afin que les normals soient calculés avant l'envoi des données à openGL.

*lemming.off* avec computeNormals : 
![](https://cdn.discordapp.com/attachments/681520398927659008/681558456112644139/lemming_compute.png "Lemming computeNormals")

Système solaire vec compute normals la couleur vtx_color (par défaut dans le fichier off) :
![](https://cdn.discordapp.com/attachments/681520398927659008/681558448051454032/terre_lune_haut.png "Systeme original color terre en haut")

![](https://cdn.discordapp.com/attachments/681520398927659008/681558454040789016/lune_terre_droite.png "Systeme lune terre droite")


