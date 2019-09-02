#ifndef __MESH_H__
#define __MESH_H__

#include <Ogre.h>
#include <OgreWireBoundingBox.h>

#include "Common.h"

#include <list>

class Mesh : public QObject
{
    Q_OBJECT
public:
    enum MeshPolygonMode {
        PM_SOLID = 0,
        PM_WIREFRAME,
        PM_POINTS
    };

    Mesh(Ogre::SceneManager* sceneMgr, Ogre::ResourceGroupManager* resGrpMgr);
    ~Mesh();

    ///
    /// Accessors
    ///

    inline Ogre::Entity* getOgreEntity() { return this->mMesh; }
    inline Ogre::SceneNode* getOgreNode() { return this->mMeshNode; }

    ///
    /// Attributes
    ///

    inline Ogre::AnimationStateSet* getAnimationStateSet() { return this->mMesh->getAllAnimationStates(); }

    inline bool isMeshLoaded()     { return this->mMeshLoaded; }
    inline bool isResourceLoaded() { return this->mResLoaded; }
    inline void showBoundingBox(bool enabled) { if (mMeshLoaded && mBoundingBox) { mBoundingBox->setVisible(enabled); } }    
    inline bool hasSkeleton() { if (mMeshLoaded) { return mMesh->hasSkeleton(); } else return false; }
    inline bool isAnimationPlaying() { return this->mAnimPlaying; }
    inline bool isAnimationLooping() { return this->mAnimLoop; }
    inline void toggleAnimationLoop() { mAnimLoop = !mAnimLoop; }
    inline MeshPolygonMode getPolyMode() { return this->mCurPolyMode; }



    void showSkeleton(bool enabled);

    ///
    /// Methods
    ///

    void load(Ogre::String meshFile, Ogre::String meshPath, bool addResPath);
    void unload();

    void updateViewState(ViewState viewState, Ogre::Real time);
    void resetViewState();

    void setPolygonMode(MeshPolygonMode mode);

    void setActiveAnimation(const Ogre::String &name);    
    void clearActiveAnimation();
    void playAnimation();
    void stopAnimation();
    void setAnimationLoop(bool loop);
    void setAnimationPosition(Ogre::Real time);

private:
    Ogre::SceneManager*         mSceneMgr;
    Ogre::ResourceGroupManager* mResGrpMgr;
    Ogre::SceneNode*            mParentNode;
    Ogre::SceneNode*            mPitchNode;
    Ogre::SceneNode*            mMeshNode;
    Ogre::SceneNode*            mSkeletonNode;
    Ogre::Entity*               mMesh;
    MeshPolygonMode             mCurPolyMode;
    Ogre::AnimationState*       mActiveAnim;
    Ogre::ManualObject*         mBoundingBox;

    // tells if a mesh has been loaded
    bool mMeshLoaded;
    // tells if a resource path has been loaded
    bool mResLoaded;

    bool mAnimPlaying;
    bool mAnimLoop;

    // holds all submesh material names, used to restore SOLID mode when
    // changing the meshes polygon mode.
    std::vector<Ogre::String> mMatNames;

    ///
    /// Private Methods
    ///

    void _resetState();
    void _createBoundingBox();
    void _updateBoundingBox(const Ogre::AxisAlignedBox aabb);
    void _createSkeleton();
    void _updateSkeleton();
    void _destroySkeleton();


protected:

signals:
    void animationChanged(float length);
    void animationPositionChanged(float position);

}; // Mesh

#endif // __MESH_H__
