#include <OgreException.h>

#include "Common.h"
#include "Mesh.h"

///
/// construction / destruction
///

Mesh::Mesh(Ogre::SceneManager* sceneMgr, Ogre::ResourceGroupManager* resGrpMgr)
{    
    assert(sceneMgr  != NULL);
    assert(resGrpMgr != NULL);

    mSceneMgr    = sceneMgr;
    mResGrpMgr   = resGrpMgr;

    this->_resetState();
} // constructor


Mesh::~Mesh()
{
    this->unload();
} // destructor

///
/// private methods
///

void Mesh::_resetState()
{
    mBoundingBox  = NULL;
    mMesh         = NULL;
    mMeshNode     = NULL;
    mSkeletonNode = NULL;
    mActiveAnim   = NULL;

    mAnimLoop    = false;
    mAnimPlaying = false;
    mMeshLoaded  = false;
    mResLoaded   = false;
}

void Mesh::_createBoundingBox()
{
    assert( mMeshNode != NULL );

    mBoundingBox = new Ogre::ManualObject(BOUNDING_BOX_NAME);
    this->_updateBoundingBox(mMesh->getBoundingBox());

    mBoundingBox->setVisible(false);
    mMeshNode->attachObject(mBoundingBox);
} // _createBoundingBox

void Mesh::_updateBoundingBox(const Ogre::AxisAlignedBox aabb)
{
    assert( mMeshNode != NULL );

    if ( mBoundingBox ) {
        qDebug() << "Redrawing box ...";
        mBoundingBox->clear();
        mBoundingBox->begin(MAT_BOUNDING_BOX, Ogre::RenderOperation::OT_LINE_LIST);

        // front
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::NEAR_LEFT_TOP)     );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::NEAR_RIGHT_TOP)    );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::NEAR_RIGHT_TOP)    );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::NEAR_RIGHT_BOTTOM) );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::NEAR_RIGHT_BOTTOM) );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::NEAR_LEFT_BOTTOM)  );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::NEAR_LEFT_BOTTOM)  );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::NEAR_LEFT_TOP)     );

        // left
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::NEAR_LEFT_TOP)    );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_TOP)     );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_TOP)     );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_BOTTOM)  );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_BOTTOM)  );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::NEAR_LEFT_BOTTOM) );

        // back
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_TOP)     );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_RIGHT_TOP)    );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_RIGHT_TOP)    );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_RIGHT_BOTTOM) );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_RIGHT_BOTTOM) );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_BOTTOM)  );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_BOTTOM)  );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_LEFT_TOP)     );

        // right
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::NEAR_RIGHT_TOP)    );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_RIGHT_TOP)     );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_RIGHT_TOP)     );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_RIGHT_BOTTOM)  );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::FAR_RIGHT_BOTTOM)  );
        mBoundingBox->position( aabb.getCorner(Ogre::AxisAlignedBox::NEAR_RIGHT_BOTTOM) );

        mBoundingBox->end();
    }
} // _updateBoundingBox

void Mesh::_createSkeleton()
{
    assert( mMesh != NULL );

    if ( mMesh->hasSkeleton() ) {

        Ogre::SkeletonInstance::BoneIterator bones = mMesh->getMesh()->getSkeleton()->getBoneIterator();

        // parent skeleton nodes to the mesh node so they will automatically be
        // destroyed when we unload the mesh
        mSkeletonNode = mMeshNode->createChildSceneNode(MESH_NODE_SKELETON);

        // walk the bones ...
        while ( bones.hasMoreElements() ) {
            Ogre::Bone* bone = bones.getNext();
            Ogre::SceneNode *boneNode = mSkeletonNode->createChildSceneNode( bone->getName() );
            Ogre::Entity* axis = mSceneMgr->createEntity(BONE_MESH_PREFIX + bone->getName(), MESH_AXES);

            boneNode->attachObject(axis);
            boneNode->setOrientation(bone->_getDerivedOrientation());
            boneNode->setPosition(bone->_getDerivedPosition());
        }

        mSkeletonNode->setVisible(false);

    } // hasSkeleton?

} // _createSkeleton

void Mesh::_updateSkeleton()
{
    assert( mMesh != NULL );

    if ( mMesh->hasSkeleton() ) {

        Ogre::SkeletonInstance::BoneIterator bones = mMesh->getSkeleton()->getBoneIterator();

        while ( bones.hasMoreElements() ) {
            Ogre::Bone* bone = bones.getNext();

            Ogre::SceneNode *boneNode = mSceneMgr->getSceneNode( bone->getName() );
            boneNode->setOrientation(bone->_getDerivedOrientation());
            boneNode->setPosition(bone->_getDerivedPosition());
        }

    } // hasSkeleton?    

} // _updateSkeleton

void Mesh::_destroySkeleton()
{
    assert( mMesh != NULL );

    if ( mMesh->hasSkeleton() ) {
        Ogre::SkeletonInstance::BoneIterator bones = mMesh->getSkeleton()->getBoneIterator();

        while ( bones.hasMoreElements() ) {
            Ogre::Bone* bone = bones.getNext();
            mSceneMgr->destroyEntity(BONE_MESH_PREFIX + bone->getName());
        }

        mSkeletonNode = NULL;
    } // hasSkeleton?

} // _destroySkeleton

///
/// public methods
///

void Mesh::load(Ogre::String meshFile, Ogre::String meshPath, bool addResPath = true)
{
    //assert( parentNode != NULL );

    if ( mMeshLoaded ) { this->unload(); }

    // add mesh path to the resource manager
    if ( addResPath ) {
        mResLoaded = true;
        mResGrpMgr->addResourceLocation(meshPath, "FileSystem", RES_GRP_AUTO);
        mResGrpMgr->initialiseResourceGroup(RES_GRP_AUTO);
    }

    // setup scene nodes and place the mesh into the scene
    try {
        mMesh = mSceneMgr->createEntity(MESH_NAME, meshFile);

        mMeshLoaded = true;
        mActiveAnim = NULL;

        // save the original material names used to restore SOLID mode when
        // changing the meshes polygon mode.
        mCurPolyMode = PM_SOLID;
        mMatNames.clear();
        for (unsigned int i=0; i<mMesh->getNumSubEntities(); i++) {
            mMatNames.push_back( mMesh->getSubEntity(i)->getMaterial()->getName() );
        }
    } catch (Ogre::Exception E) {
        // release any previously loaded items
        this->unload();
        // send the exception back up the pipe
        throw E;
    }

    mPitchNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(MESH_NODE_ROOT);
    mMeshNode  = mPitchNode->createChildSceneNode(MESH_NODE_ENTITY);

    mMeshNode->attachObject(mMesh);

    // setup inital states
    mPitchNode->setInitialState();
    mMeshNode->setInitialState();


    // setup a bounding box which can be oriented and translated with the mesh
    this->_createBoundingBox();

    // setup a skeleton which can be oriented and translated with the mesh
    if ( mMesh->hasSkeleton() ) { this->_createSkeleton(); }

} // Load

void Mesh::unload()
{
    assert( mSceneMgr != NULL );

    // IMPORTANT: This may be called during the loading process in case of
    // an error to unload any previously loaded items. Make sure any unload
    // operations verify the data they are releasing was in fact allocated.

    // release the mesh and scene nodes
    if ( mMeshLoaded ) {

        if ( mBoundingBox ) { delete mBoundingBox; }
        if ( mMesh->hasSkeleton() ) { this->_destroySkeleton(); }

        mSceneMgr->destroyEntity(MESH_NAME);

        mPitchNode->getParentSceneNode()->removeAndDestroyChild(MESH_NODE_ROOT);

        mMatNames.clear();        
    }

    // release resource group
    if ( mResLoaded ) {
        mResGrpMgr->destroyResourceGroup(RES_GRP_AUTO);

        mResLoaded = false;
    }

    this->_resetState();

} // Unload

///
/// transformations
///

void Mesh::updateViewState(ViewState viewState, Ogre::Real time)
{       
    if ( mMeshLoaded ) {

        if (mActiveAnim && mAnimPlaying ) {

            if ( mActiveAnim->hasEnded() ) {
                mAnimPlaying = false;
            } else {
                qDebug("Anim: %s Position: %f", mActiveAnim->getAnimationName().c_str(), mActiveAnim->getTimePosition());
                mActiveAnim->addTime(time);                
                emit animationPositionChanged( mActiveAnim->getTimePosition() );
            }

        }        

        mPitchNode->pitch(viewState.pitch, Ogre::Node::TS_LOCAL);
        mMeshNode->yaw(viewState.yaw, Ogre::Node::TS_LOCAL);
        mMeshNode->roll(viewState.roll, Ogre::Node::TS_PARENT);

        if ( mMesh->hasSkeleton() ) { this->_updateSkeleton(); }        
    }
} // updateViewState

void Mesh::resetViewState()
{
    if ( mMeshLoaded ) {
        mPitchNode->resetToInitialState();
        mMeshNode->resetToInitialState();

        // NOTE: any active animations must be disable here otherwise
        // the skeleton will not reset with the mesh.

        if ( mMesh->hasSkeleton() ) { this->_updateSkeleton(); }
    }
} // resetViewState

void Mesh::setPolygonMode(MeshPolygonMode mode)
{
    if ( mMeshLoaded )
    {
        switch(mode)
        {
            case PM_SOLID:
                for (unsigned int i=0; i<mMesh->getNumSubEntities(); i++) {
                    mMesh->getSubEntity(i)->setMaterialName(mMatNames[i]);
                }
                break;

            case PM_WIREFRAME:
                mMesh->setMaterialName(MAT_WIREFRAME);
                break;

            case PM_POINTS:
                mMesh->setMaterialName(MAT_POINTS);
                break;
        }
    }

    mCurPolyMode = mode;

} // setPolygonMode

void Mesh::showSkeleton(bool enabled)
{
    if (mMeshLoaded && mMesh->hasSkeleton()) {
        mSkeletonNode->setVisible(enabled);
    }
}

///
/// animation methods
///

void Mesh::setActiveAnimation(const Ogre::String &name)
{
    Ogre::AnimationStateSet* as = mMesh->getAllAnimationStates();
    if ( as ) {
        if ( as->hasAnimationState(name) ) {            

            if ( mActiveAnim ) {                
                mActiveAnim->setTimePosition(0);
                mActiveAnim->setEnabled(false);
            }

            // find a reference to the animation object, this may be a vertex animation
            // or a skeletal animation
            /*
            Ogre::Animation* anim;
            anim = mMesh->getMesh()->getAnimation(name);                        
            if ( !anim && mMesh->hasSkeleton() ) {
                // not present in vertex animations, check the skeleton
                anim = mMesh->getMesh()->getSkeleton()->getAnimation(name);
            }

            if ( anim ) {
                emit animationChanged( (ulong)anim->
            } else {
                qDebug("ERROR: Could not find animation object.");
            }
            */            
            mActiveAnim = as->getAnimationState(name);

            emit animationChanged( mActiveAnim->getLength() );


            mAnimPlaying = mAnimLoop && mAnimPlaying;
            mActiveAnim->setLoop(mAnimLoop);
            mActiveAnim->setEnabled(true);
        }
    }

}

void Mesh::clearActiveAnimation() {
    if ( mActiveAnim ) {
        mActiveAnim->setEnabled(false);
        mActiveAnim = NULL;
    }
    emit animationChanged(0);
}

void Mesh::playAnimation()
{
    if ( mActiveAnim && mActiveAnim->hasEnded() ) {
        mActiveAnim->setTimePosition(0);
    }

    mAnimPlaying = true;
}

void Mesh::stopAnimation()
{
    mAnimPlaying = false;
}

void Mesh::setAnimationLoop(bool loop)
{
    mAnimLoop = loop;

    if ( mActiveAnim ) {
        mActiveAnim->setLoop(mAnimLoop);
    }
}

void Mesh::setAnimationPosition(Ogre::Real time)
{    
    if ( mActiveAnim ) {
        qDebug("Add animation time: %f", time);
        mActiveAnim->setTimePosition(time);        
    }
}
