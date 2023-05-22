#include "graph.h"

graph::graph()
{
  graph_.reserve(1000);
}
graph::~graph(){}


int32_t graph::GetNode(Tile &t)
{
  for (int32_t i = 0; i < graph_.size(); i++)
  {
    if (graph_.at(i).tile == t)
      return i;
  }
  return -1;
}

bool IsVertexIn(Tile t, int32_t &index, const std::vector<Vertex> &graph_)
{
  for(int32_t i = 0; i < graph_.size(); i++) {
    if (graph_.at(i).tile == t)
    {
      index = i;
      return true;
    }
  }
  return false;
}

bool AuxAreAdjacent(int32_t index_from, int32_t index_to, const std::vector<Vertex> &graph_)
{
  HalfEdge *aux = graph_.at(index_from).adjacency_list;
  while (aux != NULL)
  {
    if (aux->vertex_index == index_to)
      return true;
    aux = aux->next_edge;
  }
  return false;
}

void AddHalfEdge(int32_t index_from, int32_t index_to, uint16_t weight, std::vector<Vertex> &graph_)
{
  HalfEdge *e = new HalfEdge;
  e->weight = weight;
  e->vertex_index = index_to;
  e->next_edge = graph_.at(index_from).adjacency_list;
  graph_.at(index_from).adjacency_list = e;
}

void RemoveHalfEdge(int32_t index_from, int32_t index_to, std::vector<Vertex> &graph_)
{
  /* TODO */
  for (HalfEdge *edges = graph_.at(index_from).adjacency_list; edges != nullptr; edges = edges->next_edge)
  {
    if (edges->vertex_index == index_to)
    {
      edges = NULL;
      graph_.at(index_from).adjacency_list = edges;
      return;
    }
    if (edges->next_edge->vertex_index == index_to)
    {
      edges->next_edge = edges->next_edge->next_edge;
      return;
    }
  }
}

/*******************************************************************************************************/
// Grafo
/*******************************************************************************************************/

// Aggiunge nuovo vertice con etichetta la stringa. Fallisce se gia' presente
bool graph::AddVertex(Tile t)
{
  if (GetNode(t) >= 0)
    return false;
  Vertex n = Vertex(t, NULL, false);
  graph_.push_back(n);
  return true;
}

// Aggiunge un arco di peso "w" tra i nodi con etichetta "f" e "t". Fallisce se esiste gia' l'arco
// se i nodi non esistono nel grafo e se si tenta di inserire un arco tra un nodo ed esso stesso
bool graph::AddEdge(Tile from, Tile to, uint16_t weight)
{
  if (from == to)
    return false;
  int32_t index_from;
  int32_t index_to;
  if (!IsVertexIn(from, index_from, graph_) || !IsVertexIn(to, index_to, graph_))
    return false;
  if (AuxAreAdjacent(index_from, index_to, graph_))
    return false;
  AddHalfEdge(index_from, index_to, weight, graph_);
  AddHalfEdge(index_to, index_from,  weight, graph_);
  /*
  */
  return true;
}

bool graph::RemoveEdge(Tile from, Tile to)
{
  if (from == to)
    return false;
  int32_t index_from;
  int32_t index_to;
  if (!IsVertexIn(from, index_from, graph_) || !IsVertexIn(to, index_to, graph_))
    return false;
  if (!AuxAreAdjacent(index_from, index_to, graph_))
    return false;
  RemoveHalfEdge(index_from, index_to, graph_);
  RemoveHalfEdge(index_to, index_from, graph_);
  return true;
}

// Ritorna il numero di vertici del grafo
int graph::NumVertices()
{
  return graph_.size();
}

// Ritorna il numero di archi del grafo
int graph::NumEdges()
{
  int tot = 0;
  for (int32_t i = 0; i < graph_.size(); i++)
  {
    NodeDegree(graph_.at(i).tile, tot);
  }
  return (tot / 2);
}

// Calcola e ritorna (nel secondo parametro) il grado del nodo. Fallisce
// se il nodo non esiste
bool graph::NodeDegree(Tile t, int &degree)
{
  if (GetNode(t) < 0)
    return false;
  Vertex vertex = graph_.at(GetNode(t));
  for (HalfEdge *edges = vertex.adjacency_list; edges != nullptr; edges = edges->next_edge)
  {
    ++degree;
  }
  return true;
}

// Verifica se i due vertici v1 e v2 sono adiacenti (ovvero se esiste un arco)
bool graph::AreAdjacent(Tile v1, Tile v2)
{
  int32_t index_from = -1;
  int32_t index_to = -1;
  if (!IsVertexIn(v1, index_from, graph_) || !IsVertexIn(v2, index_to, graph_))
    return false;
  return AuxAreAdjacent(index_from, index_to, graph_);
}

// Restituisce la lista di adiacenza di un vertice
std::vector<Tile> graph::GetAdjacencyList(Tile v1)
{
  std::vector<Tile> tile_vect;
  Vertex aux = graph_.at(GetNode(v1));
  HalfEdge *edges = aux.adjacency_list;
  while (edges != nullptr)
  {
    tile_vect.push_back(graph_.at(edges->vertex_index).tile);
    edges = edges->next_edge;
  }
  return tile_vect;
}

// Ritorna un cammino tra una citta' ed un altra
// Il cammino da "v1" a "v2" alla fine sara' in "path"
// e la lunghezza sara' in "len".
// Si assume che il chiamante fornisca inizialmente un cammino vuoto.
//
// La funzione rappresenta una variante della visita DFS

bool FindPathAux(int32_t here, int32_t to, std::vector<Tile> &path, int &len, std::vector<Vertex> &graph_)
{
  graph_.at(here).visited = true;
  for (HalfEdge *ee = graph_.at(here).adjacency_list; ee != nullptr; ee = ee->next_edge)
  {
    if (graph_.at(ee->vertex_index).visited)
      continue;
    if (graph_.at(ee->vertex_index).tile == graph_.at(to).tile)
    {
      path.insert(path.begin(), graph_.at(ee->vertex_index).tile);
      len += ee->weight;
      return true;
    }
    bool res = FindPathAux(ee->vertex_index, to, path, len, graph_);
    if (res)
    {
      path.insert(path.begin(), graph_.at(ee->vertex_index).tile);
      len += ee->weight;
      return true;
    }
  }
  return false;
}

void graph::FindPath(Tile v1, Tile v2, std::vector<Tile> &path, int &len)
{
  int32_t from = GetNode(v1), to = GetNode(v2);
  if (from == to || from == -1 || to == -1)
    return;
  for (int32_t i = 0; i < graph_.size(); i++)
    graph_.at(i).visited = false;
  len = 0;
  FindPathAux(from, to, path, len, graph_);
  return;
}

void graph::PrintGraph()
{
  for (int32_t i = 0; i < graph_.size(); i++)
  {
    std::cout << graph_.at(i).tile << " :: ";
    HalfEdge *adjnode = graph_.at(i).adjacency_list;
    while (adjnode != nullptr)
    {
      std::cout << graph_.at(adjnode->vertex_index).tile << " " << adjnode->weight;
      if (adjnode->next_edge == nullptr)
        break;
      std::cout << " - ";
      adjnode = adjnode->next_edge;
    }
    std::cout << std::endl;
  }
}