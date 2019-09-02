#include "Common.h"
#include "Grid.h"

///
/// Construction / Destruction
///
Grid::Grid(Ogre::SceneManager* sceneMgr)
{
    assert( sceneMgr != NULL );

    mSceneMgr = sceneMgr;

    mGrid = new Ogre::ManualObject(MESH_NAME_GRID);

    mGridNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(NODE_GRID_ROOT);
    mGridNode->attachObject(mGrid);

    mVisible = false;
} // constructor


Grid::~Grid()
{
    mSceneMgr->destroyEntity(MESH_NAME_GRID);
    mGridNode->getParentSceneNode()->removeAndDestroyChild(NODE_GRID_ROOT);    

    mGrid     = NULL;
    mGridNode = NULL;
    mSceneMgr = NULL;
} // destructor


///
/// Public Methods
///
void Grid::setSize(int cells, int size)
{
    mCellSize = size;
    mNumCells = cells;

    int offset  = (cells * size)/2;

    mGrid->clear();

    // grid lines

    mGrid->begin(MAT_GRID, Ogre::RenderOperation::OT_LINE_LIST);

    for ( int x=0; x<=cells; x++ ) {
        // skip drawing the axis to reduce flicker for these lines
        if ( x == (cells/2) ) { continue; }

        mGrid->position( Ogre::Vector3( -offset + (x*size), 0, -offset) );
        mGrid->position( Ogre::Vector3( -offset + (x*size), 0,  offset) );

        mGrid->position( Ogre::Vector3( -offset, 0, -offset + (x*size)) );
        mGrid->position( Ogre::Vector3(  offset, 0, -offset + (x*size)) );
    }

    mGrid->end();

    /*
    // Grid border
    mGrid->begin(MAT_GRID_BORDER, Ogre::RenderOperation::OT_LINE_LIST);
    // left
    mGrid->position( Ogre::Vector3( -offset, 0, -offset) );
    mGrid->position( Ogre::Vector3( -offset, 0,  offset) );
    // front
    mGrid->position( Ogre::Vector3( -offset, 0,  offset) );
    mGrid->position( Ogre::Vector3(  offset, 0,  offset) );
    // right
    mGrid->position( Ogre::Vector3(  offset, 0,  offset) );
    mGrid->position( Ogre::Vector3(  offset, 0, -offset) );
    // back
    mGrid->position( Ogre::Vector3(  offset, 0, -offset) );
    mGrid->position( Ogre::Vector3( -offset, 0, -offset) );
    mGrid->end();
    */

    // draw the axes
    mGrid->begin(MAT_GRID_AXIS_Z, Ogre::RenderOperation::OT_LINE_LIST);
    mGrid->position( Ogre::Vector3( 0, 0, -offset ) );
    mGrid->position( Ogre::Vector3( 0, 0,  offset ) );
    mGrid->end();

    mGrid->begin(MAT_GRID_AXIS_X, Ogre::RenderOperation::OT_LINE_LIST);
    mGrid->position( Ogre::Vector3( -offset, 0, 0 ) );
    mGrid->position( Ogre::Vector3(  offset, 0, 0 ) );
    mGrid->end();

    mGridNode->setVisible(mVisible);
} // setSize


void Grid::setVisible(bool visible)
{
    mGridNode->setVisible(visible);

    mVisible = visible;
} // setVisible


int Grid::clamp( double size )
{
    int i = (int)trunc(size);
    int r = 0;
    int c = 0;

    while ( i > 100 ) {
        ++c;
        i = (int)(i / 10);
    }

    r = (i % 10);
    if ( r >= 5 ) { i += ( 5 - r ); } else { i = (i - r); }
    i = (int)ceil(i * pow(10, c));
    if ( i <= 0 ) { i = 5; }

    return i;
} // clamp
