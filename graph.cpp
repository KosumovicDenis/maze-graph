#include "graph.h"

graph::graph()
{
  graph_.reserve(1000);
}
graph::~graph() {}

int32_t graph::GetNode(const Tile &t)
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
  for (int32_t i = 0; i < graph_.size(); i++)
  {
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
      HalfEdge *temp = edges;
      edges = edges->next_edge;
      graph_.at(index_from).adjacency_list = edges;
      delete temp;
      return;
    }
    if (edges->next_edge->vertex_index == index_to)
    {
      HalfEdge *temp = edges->next_edge;
      edges->next_edge = edges->next_edge->next_edge;
      delete temp;
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
  Vertex n = Vertex(t, nullptr, false);
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
  AddHalfEdge(index_to, index_from, weight, graph_);
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
    std::cout << "(" << graph_.at(i).tile << ") |->| ";
    HalfEdge *adjnode = graph_.at(i).adjacency_list;
    while (adjnode != nullptr)
    {
      std::cout << "(" << graph_.at(adjnode->vertex_index).tile << ")" << " <- Weight: " << adjnode->weight;
      if (adjnode->next_edge == nullptr)
        break;
      std::cout << " || ";
      adjnode = adjnode->next_edge;
    }
    std::cout << std::endl;
  }
}

// TODO: Creare un'array ordinato (0,n) e poi (1,n) e così via...
// Stampare gli elementi presenti nell'array nelle colonne indicate
void graph::PrintMaze()
{
  std::vector<Tile> ordered_nodes;
  ordered_nodes.reserve(1000);
  for (int32_t i = 0; i < graph_.size(); i++)
  {
    int32_t temp_index = 0;
    for (int32_t j = 0; j < ordered_nodes.size(); j++)
    {
      if (graph_.at(i).tile < ordered_nodes.at(j))
      {
        break;
      }
      temp_index++;
    }
    ordered_nodes.insert(ordered_nodes.begin() + temp_index, graph_.at(i).tile);
  }

  int32_t max_x = ordered_nodes.at(0).x;
  int32_t min_x = ordered_nodes.at(0).x;
  int32_t max_y = ordered_nodes.at(0).y;
  for (int i = 0; i < ordered_nodes.size(); i++)
  {
    if (max_x < ordered_nodes.at(i).x)
      max_x = ordered_nodes.at(i).x;
    if (min_x > ordered_nodes.at(i).x)
      min_x = ordered_nodes.at(i).x;
    if (max_y < ordered_nodes.at(i).y)
      max_y = ordered_nodes.at(i).y;
  }

  for (int8_t y = ordered_nodes.at(0).y; y <= max_y; y++)
  {
    for (int8_t i = 0; i < 3; i++)
    {
      for (int8_t x = min_x; x <= max_x; x++)
      {
        if (i == 0)
        {
          if (GetNode({y, x}) == -1)
          {
            if (GetNode({y-1, x}) != -1)
            {
              std::cout << "+---";
            }
            else
            {
              if (GetNode({y, x-1}) != -1)
              {
                std::cout << "+   ";
              }
              else
              {
                std::cout << "    ";
              }
            }
          }
          else
          {
            if (AreAdjacent({y, x}, {y-1, x}))
            {
              if (AreAdjacent({y, x}, {y, x-1}) && AreAdjacent({y, x-1}, {y-1, x-1}) && AreAdjacent({y-1, x-1}, {y-1, x}))
              {
                std::cout << " ";
                std::cout << "   ";
              }
              else
              {
                std::cout << "+";
                std::cout << "   ";
              }
            }
            else
            {
              std::cout << "+";
              std::cout << "---";
            }
          }
        }
        else if (i == 1)
        {
          if (GetNode({y, x}) == -1)
          {
            if (GetNode({y, x-1}) != -1)
            {
              std::cout << "|   ";
            }
            else
            {
              std::cout << "    ";
            }
          }
          else
          {
            if (AreAdjacent({y, x}, {y, x-1}))
            {
              std::cout << "    ";
            }
            else
            {
              std::cout << "|";
              std::cout << "   ";
            }
          }
        }
      }
      if (i == 0)
      {
        if (GetNode({y, max_x}) != -1 || GetNode({y-1, max_x}) != -1) 
          std::cout << "+\n";
        else
          std::cout << "\n";
      }
      else if (i == 1)
      {
        if (GetNode({y, max_x}) != -1)
          std::cout << "|\n";
        else
          std::cout << "\n";
      }
    }
  }
  for (int8_t x = min_x; x <= max_x; x++)
  {
    if (GetNode({max_y, x}) == -1)
    {
      if (GetNode({max_y, x-1}) == -1)
      {
        std::cout << "    ";
      }
      else
      {
        std::cout << "+   ";
      }
    }
    else
    {
      std::cout << "+---";
    }
  }
  if (GetNode({max_y, max_x}) != -1) 
    std::cout << "+\n";
  else
    std::cout << "\n";
  std::cout << std::endl;
}

/*
func (m *Maze) Print(solve bool) string {
	out := "+"
	for i := 0; i < m.Width; i++ {
		out = fmt.Sprintf("%s%s", out, "---+")
	}
	out = fmt.Sprintf("%s\n", out)
	for y := m.Length - 1; y >= 0; y-- {
		top := "|"
		bottom := "+"
		for x := 0; x < m.Width; x++ {
			body := "   "
			if m.Solution != nil && solve {
				if _, ok := m.Solution.path[point{x: x, y: y}]; ok {
					if x == m.Solution.EntranceX && y == m.Solution.EntranceY {
						body = color.RGB(205, 0, 0).Sprintf(" o ")
					} else if x == m.Solution.ExitX && y == m.Solution.ExitY {
						body = color.RGB(0, 205, 0).Sprintf(" o ")
					} else {
						dist := float32(m.GetDistance(x, y)) / float32(m.Solution.MaxDistance)
						R := math.Max(208*(1-float64(dist))+48, 0)
						G := math.Max(208*(0.9-1.4*float64(dist))+48, 0)
						B := math.Max(208*(0.9-1.4*float64(dist))+48, 0)
						body = color.RGB(uint8(R), uint8(G), uint8(B)).Sprintf(" o ")
					}
				}
			}
			cell := m.cells[x][y]
			east := "|"
			if cell.IsLinkedEast() {
				east = " "
			}
			top = fmt.Sprintf("%s%s%s", top, body, east)
			south := "---"
			if cell.IsLinkedSouth() {
				south = "   "
			}
			bottom = fmt.Sprintf("%s%s+", bottom, south)
		}
		out = fmt.Sprintf("%s%s\n%s\n", out, top, bottom)
	}
	return out
}
*/