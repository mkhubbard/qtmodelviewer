#ifndef __GRID_H__
#define __GRID_H__

#include "Common.h"

#include <Ogre.h>

class Grid
{
    private:
        Ogre::SceneManager*  mSceneMgr;
        Ogre::ManualObject*  mGrid;
        Ogre::SceneNode*     mGridNode;

        int mCellSize;
        int mNumCells;
        bool mVisible;

    protected:
        // none
    public:

        Grid(Ogre::SceneManager *sceneMgr);
        ~Grid();

        void setSize(int cells, int size);
        void setVisible(bool visible);
        int clamp( double size );

        ///
        /// Accessors
        ///

        inline Ogre::SceneNode* getOgreNode() { return mGridNode; }
        inline int getCellSize() { return mCellSize; }
        inline int getNumCells() { return mNumCells; }
        inline bool isVisible() { return mVisible; }

}; // Grid

#endif // __GRID_H__
