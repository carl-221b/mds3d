
#include "bvh.h"
#include "mesh.h"
#include <iostream>

void BVH::build(const Mesh* pMesh, int targetCellSize, int maxDepth)
{
    // store a pointer to the mesh
    m_pMesh = pMesh;
    // allocate the root node
    m_nodes.resize(1);

    if(m_pMesh->nbFaces() <= targetCellSize) { // only one node
        m_nodes[0].box = pMesh->AABB();
        m_nodes[0].first_face_id = 0;
        m_nodes[0].is_leaf = true;
        m_nodes[0].nb_faces = m_pMesh->nbFaces();
        m_faces.resize(m_pMesh->nbFaces());
        for(int i=0; i<m_pMesh->nbFaces(); ++i)
        {
            m_faces[i] = i;
        }
    }else{
        // reserve space for other nodes to avoid multiple memory reallocations
        m_nodes.reserve( std::min<int>(2<<maxDepth, std::log(m_pMesh->nbFaces()/targetCellSize) ) );

        // compute centroids and initialize the face list
        m_centroids.resize(m_pMesh->nbFaces());
        m_faces.resize(m_pMesh->nbFaces());
        for(int i=0; i<m_pMesh->nbFaces(); ++i)
        {
            m_centroids[i] = (m_pMesh->vertexOfFace(i, 0).position + m_pMesh->vertexOfFace(i, 1).position + m_pMesh->vertexOfFace(i, 2).position)/3.f;
            m_faces[i] = i;
        }

        // recursively build the BVH, starting from the root node and the entire list of faces
        buildNode(0, 0, m_pMesh->nbFaces(), 0, targetCellSize, maxDepth);
    }
    cout << m_nodes.size() << endl;
}

bool BVH::intersect(const Ray& ray, Hit& hit) const
{
    // compute the intersection with the root node
    float tMin,tMax;//tMin1, tMax1,tMin2,tMax2;
    Normal3f n;
    bool ret=false;
    if(::intersect(ray,m_nodes[0].box,tMin,tMax,n))
      ret=intersectNode(0,ray,hit);

    // TODO
    // vérifier si on a bien une intersection (en fonction de tMin, tMax, et hit.t()), et si oui appeler intersecNode...

    return ret;
}

bool BVH::intersectNode(int nodeId, const Ray& ray, Hit& hit) const
{
    // TODO, deux cas: soit mNodes[nodeId] est une feuille (il faut alors intersecter les triangles du noeud),
    // soit c'est un noeud interne (il faut visiter les fils (ou pas))
    bool ret=false;
    Normal3f n;
    const Node& node = m_nodes[nodeId];
    if(node.is_leaf){
      Hit hitTmp;
      for(int i=node.first_face_id; i<node.nb_faces+node.first_face_id; i++){
        if(m_pMesh->intersectFace(ray,hitTmp,m_faces[i])){
          if(hit.t()>hitTmp.t()){
            hit.setT(hitTmp.t());
            hit.setNormal(hitTmp.normal());
            hit.setShape(hitTmp.shape());
            hit.setTexcoord(hitTmp.texcoord());
            ret=true;
          }
        }
      }
    }

    else{
      float tMin1,tMin2,tMax1,tMax2;
      bool b1=::intersect(ray, m_nodes[node.first_child_id].box, tMin1, tMax1, n);
      bool b2=::intersect(ray, m_nodes[node.first_child_id+1].box, tMin2, tMax2, n);
      //Si on intersecte les deux boites et qu'elles s'emboitent
      if(b1 && b2 && tMin2< tMax1 ){
        //On teste fils gauche et droite
          bool t1=intersectNode(node.first_child_id,ray,hit);
          bool t2=intersectNode(node.first_child_id+1,ray,hit);
          ret= t1 || t2;

      }
      //Si les boites s'emboitent pas
      else {
        //Si on intersecte le fils gauche
        if(b1 ){
          ret=intersectNode(node.first_child_id,ray,hit);
          //si on intersecte fils droite et qu'on a trouvé aucune face dans fils gauche
          if (b2 && !ret )
            ret=intersectNode(node.first_child_id+1,ray,hit);
        }
        //si on intersecte juste le fils droit
        if(!b1 && b2 ){
            ret=intersectNode(node.first_child_id+1,ray,hit);
        }
      }
    }

    return ret;
}

/** Sorts the faces with respect to their centroid along the dimension \a dim and spliting value \a split_value.
  * \returns the middle index
  */
int BVH::split(int start, int end, int dim, float split_value)
{
    int l(start), r(end-1);
    while(l<r)
    {
        // find the first on the left
        while(l<end && m_centroids[l](dim) < split_value) ++l;
        while(r>=start && m_centroids[r](dim) >= split_value) --r;
        if(l>r) break;
        std::swap(m_centroids[l], m_centroids[r]);
        std::swap(m_faces[l], m_faces[r]);
        ++l;
        --r;
    }
    return m_centroids[l][dim]<split_value ? l+1 : l;
}

void BVH::buildNode(int nodeId, int start, int end, int level, int targetCellSize, int maxDepth)
{
    Node& node = m_nodes[nodeId];

    // étape 1 : calculer la boite englobante des faces indexées de m_faces[start] à m_faces[end]
    // (Utiliser la fonction extend de Eigen::AlignedBox3f et la fonction mpMesh->vertexOfFace(int) pour obtenir les coordonnées des sommets des faces)
    Eigen::AlignedBox3f box;
    for(int i=start; i<end;i++){
        box= box.extend(m_pMesh->vertexOfFace(m_faces[i],0).position);
        box= box.extend(m_pMesh->vertexOfFace(m_faces[i],1).position);
        box= box.extend(m_pMesh->vertexOfFace(m_faces[i],2).position);
    }
    node.box=box;
    node.nb_faces=end-start;

    // étape 2 : déterminer si il s'agit d'une feuille (appliquer les critères d'arrêts)
    if(level==maxDepth || (end-start)<=targetCellSize ){
      node.is_leaf=true;
    // Si c'est une feuille, finaliser le noeud et quitter la fonction
      node.first_face_id=start;
      return;
    }
    // Si c'est un noeud interne :
    // étape 3 : calculer l'index de la dimension (x=0, y=1, ou z=2) et la valeur du plan de coupe
    // (on découpe au milieu de la boite selon la plus grande dimension)

    else{
    Vector3f minv= node.box.min();
    Vector3f maxv= node.box.max();

    int axisDim=0;
    float maxf=maxv.x();
    float minf=minv.x();
    if(maxf-minf<maxv.y()-minv.y()){
      axisDim=1;
      maxf=maxv.y();
      minf=minv.y();
      if(maxf-minf<maxv.z()-minv.z()){
        axisDim=2;
        maxf=maxv.z();
        minf=minv.z();
      }
    }


    // étape 4 : appeler la fonction split pour trier (partiellement) les faces et vérifier si le split a été utile
    float axisValue=(minf+maxf)*0.5;

    int s=split(start,end,axisDim,axisValue);
    // étape 5 : allouer les fils, et les construire en appelant buildNode...

    if(s==start || s==end){
      node.is_leaf=true;
      node.first_face_id=start;
      return;
    }

    int nodeSize=m_nodes.size();
    node.first_child_id=nodeSize;
    m_nodes.resize(nodeSize+2);

    buildNode(node.first_child_id,start,s,level+1,targetCellSize,maxDepth);

    buildNode(node.first_child_id+1,s,end,level+1,targetCellSize,maxDepth);
  }
}
