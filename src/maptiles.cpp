/**
 * @file maptiles.cpp
 * Code for the maptiles function.
 */

#include <iostream>
#include <map>
#include "mosaiccanvas.h"

#include "maptiles.h"

using namespace std;


Point<3> convertToXYZ(LUVAPixel pixel) {
    return Point<3>( pixel.l, pixel.u, pixel.v );
}

/*** @todo Implement this function!*/
MosaicCanvas* mapTiles(SourceImage const& theSource, vector<TileImage>& theTiles)
{
    //if (&theSource == NULL || &theTiles == NULL) return NULL;
    //Iniitializing the toReturn canvas
    MosaicCanvas* canvas = new MosaicCanvas(theSource.getRows(), theSource.getColumns());
    

    //Creating a mirrored vector of theTiles with its average values as points
    vector<Point<3>> thePts = vector<Point<3>>(theTiles.size());
    map<Point<3>, int> ref;

    for (uint i = 0; i < theTiles.size(); i++) {
        thePts[i] = convertToXYZ(theTiles[i].getAverageColor());
        ref.insert(std::pair<Point<3>, int>(convertToXYZ(theTiles[i].getAverageColor()), i));
    }

    //creating a KDTree of all average points
    KDTree<3> tree = KDTree<3>(thePts);

    //Finding the nearest neighbor for each point in canvas
    for (int i = 0; i < canvas->getRows(); i++) {
        for (int j = 0; j < canvas->getColumns(); j++) {
            Point<3> target = convertToXYZ(theSource.getRegionColor(i,j));
            Point<3> best = tree.findNearestNeighbor(target);

            //Initially used a ref to input but changed to heap after segfaults
            canvas->setTile(i, j, &theTiles[ref[best]]);
        }
    }
    return canvas;
}

