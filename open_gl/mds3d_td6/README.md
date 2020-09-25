# TD6 - Textures

##  1. Quelques remarques
Toutes les photos présentes dans ce readme sont disponibles à la racine de l'archive dans le dossier "resultats".
Le code rendu est chargé avec cow.obj et la génération procédurale de MipMap. Pour visualiser les étapes avant cela du TP j'ai laissé des lignes en commentaire qu'il faut décommenter dans viewer.cpp (bind sample, envoi de lightdir vers le shader) ainsi que dans fragment shader (texture earth,plane,cow).

## 2. Bilan 

### 2.1 Placage Textures

#### Chargement premiers textures
Utilisation de vtx_texcoords en composante rouge et verte de la couleur.

![](https://cdn.discordapp.com/attachments/681788293477564427/681788379683094538/coord_tex.png "texcoords color")

Application de la texture "earth.jpg" sur la sphere :

![](https://cdn.discordapp.com/attachments/681788293477564427/681796441823641649/globe_terrestre_2_1.png "earth texture")


#### GLSL Mix

L'interpolation se calcule avec la formule suivante :
$$e\_c = (1- \alpha) * c1 + \alpha * c2 $$

où c1 est la texture *earth.jpg* et c2 la texture *earth_nuage.png*

A l'appel de la fonction mix dans glsl je lui donne en troisième paramètre directement la couleur du nuage qui constitue la ratio terre-nuage.
Photo nuage :

Ensuite je rappelle mix sur e_c et la texture *earth_night.jpg* avec $max(n.l,0)$ en troisième paramètre.
Ce qui donne les résultats suivants : 

![](https://cdn.discordapp.com/attachments/681788293477564427/681802940305899559/cloudy_day.png " cloudy day mixed ")

![](https://cdn.discordapp.com/attachments/681788293477564427/681802935625056284/earth_night.png "earth_night")

![](https://cdn.discordapp.com/attachments/681788293477564427/681802938569719818/day_night.png "half day half night")

Par défaut dans le code la matrice en produit avec le vecteur lightDir est la même que celle des normale ce qui fait que les zones qui sont de jour restent les mêmes quand on fait tourner la planète.
Si on veut remédier à cela et faire en sorte que la source de lumière fasse office de soleil il suffit de multiplier le vecteur lightDir par la matrice de vue de la camera. De ce fait seules les zones éclairées seront de jour et le reste seront de nuit. (J'ai laissé en commentaire l'une des lignes si besoin)

### 2.2 Filtrage

Par défaut le min et mag sont sur GL_NEAREST. Voici les binding que j'ai appliqué : 
```
UP : _minfilter = GL_LINEAR_MIPMAP_NEAREST
DOWN : _minfilter = GL_NEAREST_MIPMAP_LINEAR
LEFT : _minfilter = GL_LINEAR_MIPMAP_LINEAR
RIGHT : _minfilter = GL_NEAREST_MIPMAP_NEAREST
PAGE_UP : _magfilter = GL_LINEAR
PAGE_DOWN : _magfilter = GL_NEAREST
```
####  1.2.1 Earth
* Vue zoomé magfilter NEAREST :
![](https://cdn.discordapp.com/attachments/681788293477564427/682293798411173889/earth_mag_nearest.png)
* Vue zoomé magfilter LINEAR :
![](https://cdn.discordapp.com/attachments/681788293477564427/682293134335541258/earth_mag_linear.png)
On voit sur le rendu Linear des pixels plus lisses.

* Vue dézoomé min en LINEAR_LINEAR et mag en LINEAR :
![](https://cdn.discordapp.com/attachments/681788293477564427/681829842211045387/earth_all_linear.png "dezoom linear")

En vue zoomé l'image la texture est plus lisse quand minfilter est en linear.
En vue dézoomé on remarque une petite différence quand magfilter est en linear. Cas observés également sur les cas intermédiaires.
#### 2.2.2 Plane :
* NEAREST_NEAREST : ![](https://cdn.discordapp.com/attachments/681788293477564427/681829836901056528/plane_all_nearest.png "NEAREST_NEAREST")
* LINEAR_LINEAR : ![](https://cdn.discordapp.com/attachments/681788293477564427/681829839547793447/plane_all_linear.png "LINEAR_LINEAR")

Les carreaux proches sont lissés quand magfilter est en linear.
Ceux qui sont à l'infini le sont quand minfilter est en linear_linear. Ces comportements ont été identifié sur les cas intermédiaires (NEAREST_LINEAR,LINEAR_NEAREST).

#### 2.2.3 Cow
* NEAREST_NEAREST & magfilter nearest![](https://cdn.discordapp.com/attachments/681788293477564427/681829843910000665/cow_all_nearest.png "COW NEAREST")
*  LINEAR_LINEAR & magfilter linear
![](https://cdn.discordapp.com/attachments/681788293477564427/681829845185200133/cow_all_linear.png "COW ALL LINEAR")
Les pates deviennent grises pour linear_linear.

### 2.2.Bis Texture MipMap
On remarque que pour GL_*_MIPMAP_* où * valent respectivement linear ou nearest. Le premier * fait un lissage entre les pixels de texture et le deuxieme entre les couches du mipmap.
* Même résultat  pour GL_LINEAR_NEAREST & GL_NEAREST_NEAREST :
![](https://cdn.discordapp.com/attachments/681788293477564427/681829841057742848/cow_couche_nearest.png "nearest cow proc")
* Même resultat pour GL_LINEAR_LINEAR & GL_NEAREST_LINEAR :
![](https://cdn.discordapp.com/attachments/681788293477564427/681829842484068364/cow_couche_linear.png "cow proc linear")
Changer magfilter à prioris n'influe pas.


