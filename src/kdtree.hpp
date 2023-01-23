/**
 * @file kdtree.cpp
 * Implementation of KDTree class.
 */

#include <utility>
#include <algorithm>
#include <stdexcept>

using namespace std;

// checks if first is smaller than second
template <int Dim> /*** @todo Implement this function!*/
bool KDTree<Dim>::smallerDimVal(const Point<Dim>& first, const Point<Dim>& second, int curDim) const 
{
  return (first[curDim] == second[curDim])? first < second : first[curDim] < second[curDim];
}

template <int Dim>    /*** @todo Implement this function!*/ //TIE BREAKING MIGHT BE FUCKED
bool KDTree<Dim>::shouldReplace(const Point<Dim>& target, const Point<Dim>& currentBest, const Point<Dim>& potential) const
{
  double cbDist = SquaredDist(target, currentBest);
  double pDist = SquaredDist(target, potential);
  
  return (pDist == cbDist)? potential < currentBest : pDist < cbDist;
}

template <int Dim>    /*** @todo Implement this function!*/
KDTree<Dim>::KDTree(const vector<Point<Dim>>& newPoints)
{
  vector<Point<Dim>> vect = newPoints;
  root = TreeBuilder(vect, 0, 0, vect.size() - 1);

}

template <int Dim> // recursive function that builds up the tree
typename KDTree<Dim>::KDTreeNode* KDTree<Dim>::TreeBuilder(vector<Point<Dim>>& vect, int dimension, unsigned l, unsigned r)
{
  //std::cout << "On line " << __LINE__ << " l: " << l << " r " << r << std::endl;
  //if (r >= vect.size()) std::cout << "R is now fucked!" << std::endl;
  if (l > r || r >= vect.size() || vect.empty()) return NULL;

  if (l == r) return new KDTreeNode(vect[l]);
  unsigned midpt = (l + r) / 2; 
  //std::cout << "MIDPT: " << midpt << std::endl;
  Point<Dim> temp = Quickselect(vect, dimension, l, r, midpt);
  KDTreeNode* root = new KDTreeNode(temp);
  size++;
  dimension = (++dimension) % Dim;
  //protecting midpt bounds
root->left = TreeBuilder(vect, dimension, l, midpt - 1);

  root->right = TreeBuilder(vect, dimension, midpt + 1, r);

  return root;
}

template <int Dim> // Finds the pivot index and sorts the vector based on the index
unsigned KDTree<Dim>::Partition(vector<Point<Dim>>& vect, int dimension, unsigned l, unsigned r, unsigned k)
{
  Point<Dim> pivot = vect[k];
  // Moves pivot to the right end
  std::swap(vect[k], vect[r]);

  unsigned storedIdx = l;
  // moving through the vect moving those that's outta place
  for (unsigned i = l; i < r; i++) {
    if (smallerDimVal(vect[i], pivot, dimension)) {
      std::swap(vect[storedIdx], vect[i]);
      storedIdx++;
    }
  }
  std::swap(vect[r], vect[storedIdx]);
  return storedIdx;
}

//Write Quickselect
template <int Dim> 
Point<Dim> KDTree<Dim>::Quickselect(vector<Point<Dim>>& vect, int dimension, unsigned l, unsigned r, unsigned k)
{
  if (l == r) return vect[l];
  unsigned pivotIndex = Partition(vect, dimension, l, r, k);
  if (k == pivotIndex) {
    return vect[k]; //If k stays the same we know that the list is sorted and it's in the right spot
  } else if (k < pivotIndex) {
    return Quickselect(vect, dimension, l, pivotIndex - 1, k); //searches for the right k on the left of the list
  } else {
    return Quickselect(vect, dimension, pivotIndex + 1, r, k); // searches for the right k on the right of the lsit
  }
}






template <int Dim> /*** @todo Implement this function! */
KDTree<Dim>::KDTree(const KDTree<Dim>& other)
{
  root = CopyTree(other.root);
}

template <int Dim>  //Recursive Helper function to copy a tree
typename KDTree<Dim>::KDTreeNode* KDTree<Dim>::CopyTree(KDTreeNode* root)
{
  if (!root) return NULL;
  KDTreeNode* temp = new KDTreeNode(root);
  size++;
  if (root->left) temp->left = CopyTree(root->left);
  if (root->right) temp->right = CopyTree(root->right);
  return temp;
}




template <int Dim> /*** @todo Implement this function! */
const KDTree<Dim>& KDTree<Dim>::operator=(const KDTree<Dim>& rhs)
{
  if (&rhs == this) return *this;
  this->~KDTree();
  root = CopyTree(rhs.root);
  return *this;
}

template <int Dim> /*** @todo Implement this function!*/
KDTree<Dim>::~KDTree()
{
  Clear(root);
  size = 0;
}

template <int Dim>
void KDTree<Dim>::Clear(KDTreeNode* root)
{
  if (!root) return;
  Clear(root->left);
  Clear(root->right);
  delete root;
}

template <int Dim> /*** @todo Implement this function!*/
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim>& query) const
{
  Point<Dim> temp = query;
  return Find(temp, root, 0);
}

template <int Dim>
Point<Dim> KDTree<Dim>::Find(Point<Dim>& query, KDTreeNode* curr, int dimension) const
{
  if (!curr) {
    std::cout << "Find got a NULL curr" << std::endl;
    return Point<Dim>();
  }

  if (!curr->left && !curr->right) return curr->point;
  Point<Dim> nearest = curr->point;
  bool searchLeft = false;
  
  //Traversing the subtrees to find nearest
  if (smallerDimVal(query, curr->point, dimension)) {
    if (curr->left) nearest = Find(query, curr->left, (dimension + 1) % Dim);
    searchLeft = true;
  } else if (curr->right) nearest = Find(query, curr->right, (dimension + 1) % Dim);

  //Replace if curr is closer than nearest
  if (shouldReplace(query, nearest, curr->point)) nearest = curr->point;

  double radius = SquaredDist(query, nearest);
  double splitDist = (curr->point[dimension] - query[dimension]) * (curr->point[dimension] - query[dimension]);

  //Checks if we have other potentials in different boxes
  if (splitDist <= radius) {
    Point<Dim> temp = curr->point; //TEMP MIGHT NEVER BE INITIALIZED
    if (searchLeft && curr->right) temp = Find(query, curr->right, (dimension + 1) % Dim);
    else if (!searchLeft && curr->left) temp = Find(query, curr->left, (dimension + 1) % Dim);
    if (shouldReplace(query, nearest, temp)) nearest = temp;
  }
  return nearest;
}


template <int Dim>
double KDTree<Dim>::SquaredDist(const Point<Dim>& a, const Point<Dim>& b) const {
  double toReturn = 0;
  for (int i = 0; i < Dim; i++) toReturn += ((a[i] - b[i]) * (a[i] - b[i]));
  return toReturn;
}