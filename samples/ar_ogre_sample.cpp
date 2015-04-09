#include "orsens.h"
#include "Ogre.h"
#include <OIS/OIS.h>

using namespace std;

Orsens orsens;

static void printCommandLineParams()
{
    //cout << "-mode     mode: 0 - mono, 1 - stereo" << endl;
    cout << "-size     marker size in metres (default 0.04)" << endl;
}
#define MAX_MARKERS 30 // maximun number of markers we assume they will be detected simultaneously
/// Ogre general variables
Ogre::Root* root;
OIS::InputManager* im;
OIS::Keyboard* keyboard;

/// Ogre background variables
Ogre::PixelBox mPixelBox;
Ogre::TexturePtr mTexture;

/// Ogre scene variables
Ogre::SceneNode* ogreNode[MAX_MARKERS];
Ogre::AnimationState *baseAnim[MAX_MARKERS], *topAnim[MAX_MARKERS];

/// ArUco variables
aruco::CameraParameters CameraParams, CameraParamsUnd;
Mat leftUnd;

int initOgreAR(aruco::CameraParameters camParams, unsigned char* buffer, std::string resourcePath)
{
	/// INIT OGRE FUNCTIONS
#ifdef _WIN32
  	root = new Ogre::Root(resourcePath + "plugins_win.cfg", resourcePath + "ogre.cfg");
#else
	root = new Ogre::Root(resourcePath + "plugins.cfg", resourcePath + "ogre.cfg");
#endif	
  	if (!root->showConfigDialog()) return -1;
	Ogre::SceneManager* smgr = root->createSceneManager(Ogre::ST_GENERIC);


	/// CREATE WINDOW, CAMERA AND VIEWPORT
    Ogre::RenderWindow* window = root->initialise(true);
	Ogre::Camera *camera;
	Ogre::SceneNode* cameraNode;
	camera = smgr->createCamera("camera");
	camera->setNearClipDistance(0.01f);
	camera->setFarClipDistance(10.0f);
	camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	camera->setPosition(0, 0, 0);
	camera->lookAt(0, 0, 1);
	double pMatrix[16];
	camParams.OgreGetProjectionMatrix(camParams.CamSize,camParams.CamSize, pMatrix, 0.05,10, false);
	Ogre::Matrix4 PM(pMatrix[0], pMatrix[1], pMatrix[2] , pMatrix[3],
			pMatrix[4], pMatrix[5], pMatrix[6] , pMatrix[7],
			pMatrix[8], pMatrix[9], pMatrix[10], pMatrix[11],
			pMatrix[12], pMatrix[13], pMatrix[14], pMatrix[15]);
	camera->setCustomProjectionMatrix(true, PM);
	camera->setCustomViewMatrix(true, Ogre::Matrix4::IDENTITY);
	window->addViewport(camera);
	cameraNode = smgr->getRootSceneNode()->createChildSceneNode("cameraNode");
	cameraNode->attachObject(camera);


	/// CREATE BACKGROUND FROM CAMERA IMAGE
	int width = camParams.CamSize.width;
	int height = camParams.CamSize.height;
	// create background camera image
	mPixelBox = Ogre::PixelBox(width, height, 1, Ogre::PF_R8G8B8, buffer);
	// Create Texture
	mTexture = Ogre::TextureManager::getSingleton().createManual("CameraTexture",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		      Ogre::TEX_TYPE_2D,width,height,0,Ogre::PF_R8G8B8,Ogre::TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

	//Create Camera Material
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("CameraMaterial", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::Technique *technique = material->createTechnique();
	technique->createPass();
	material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
	material->getTechnique(0)->getPass(0)->createTextureUnitState("CameraTexture");

	Ogre::Rectangle2D* rect = new Ogre::Rectangle2D(true);
	rect->setCorners(-1.0, 1.0, 1.0, -1.0);
	rect->setMaterial("CameraMaterial");

	// Render the background before everything else
	rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);

	// Hacky, but we need to set the bounding box to something big, use infinite AAB to always stay visible
	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();
	rect->setBoundingBox(aabInf);

	// Attach background to the scene
	Ogre::SceneNode* node = smgr->getRootSceneNode()->createChildSceneNode("Background");
	node->attachObject(rect);


	/// CREATE SIMPLE OGRE SCENE
	// add sinbad.mesh
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(resourcePath + "Sinbad.zip", "Zip", "Popular");
 	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	for(int i=0; i<MAX_MARKERS; i++) {
	  Ogre::String entityName = "Marker_" + Ogre::StringConverter::toString(i);
	  Ogre::Entity* ogreEntity = smgr->createEntity(entityName, "Sinbad.mesh");
	  Ogre::Real offset = ogreEntity->getBoundingBox().getHalfSize().y;
	  ogreNode[i] = smgr->getRootSceneNode()->createChildSceneNode();
	  // add entity to a child node to correct position (this way, entity axis is on feet of sinbad)
	  Ogre::SceneNode *ogreNodeChild = ogreNode[i]->createChildSceneNode();
	  ogreNodeChild->attachObject(ogreEntity);
	  // Sinbad is placed along Y axis, we need to rotate to put it along Z axis so it stands up over the marker
	  // first rotate along X axis, then add offset in Z dir so it is over the marker and not in the middle of it
	  ogreNodeChild->rotate(Ogre::Vector3(1,0,0), Ogre::Radian(Ogre::Degree(90)));
	  ogreNodeChild->translate(0,0,offset,Ogre::Node::TS_PARENT);
	  // mesh is too big, rescale!
	  const float scale = 0.006675f;
	  ogreNode[i]->setScale(scale, scale, scale);

	    // Init animation
	  ogreEntity->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);
	  if(i==0)
	  {
        baseAnim[i] = ogreEntity->getAnimationState("HandsClosed");
        topAnim[i] = ogreEntity->getAnimationState("HandsRelaxed");
	  }
	  else if(i==1)
	  {
        baseAnim[i] = ogreEntity->getAnimationState("Dance");
        topAnim[i] = ogreEntity->getAnimationState("Dance");
	  }
	  else if(i==2)
	  {
        baseAnim[i] = ogreEntity->getAnimationState("RunBase");
        topAnim[i] = ogreEntity->getAnimationState("RunTop");
	  }
	  else
	  {
        baseAnim[i] = ogreEntity->getAnimationState("IdleBase");
        topAnim[i] = ogreEntity->getAnimationState("IdleTop");
	  }
	  baseAnim[i]->setLoop(true);
	  topAnim[i]->setLoop(true);
	  baseAnim[i]->setEnabled(true);
	  topAnim[i]->setEnabled(true);
	}


 	/// KEYBOARD INPUT READING
 	size_t windowHnd = 0;
 	window->getCustomAttribute("WINDOW", &windowHnd);
 	im = OIS::InputManager::createInputSystem(windowHnd);
 	keyboard = static_cast<OIS::Keyboard*>(im->createInputObject(OIS::OISKeyboard, true));

	return 1;
}

int main( int argc, char **argv )
{
    uint8_t mode=1;
    float marker_size = 0.04;

    for( int i = 1; i < argc; i++ )
    {
        if( !strcmp( argv[i], "--help" ) || !strcmp( argv[i], "-h" ) )
        {
            printCommandLineParams();
            exit(0);
        }
        else if( !strcmp( argv[i], "-mode" ) )
        {
            mode = atoi(argv[i+1]);
        }
        else if( !strcmp( argv[i], "-size" ) )
        {
            marker_size = atof(argv[i+1]);
        }
    }

    int capture_mode = Orsens::CAPTURE_LEFT_ONLY;

    if (mode==1)
        capture_mode = Orsens::CAPTURE_LEFT_RIGHT;

    if (!orsens.start((Orsens::CaptureMode)capture_mode))
    {
        printf("unable to start\n");
        return -1;
    }

    orsens.initAR(marker_size);

    orsens.grabSensorData();
    Mat left = orsens.getLeft();
    undistort(left,leftUnd,orsens.getARCameraParametres(0).CameraMatrix,orsens.getARCameraParametres(0).Distorsion);

    CameraParams = orsens.getARCameraParametres(0);
    /// CREATE UNDISTORTED CAMERA PARAMS
    CameraParamsUnd=CameraParams;
    CameraParamsUnd.Distorsion=cv::Mat::zeros(4,1,CV_32F);

    /// INIT OGRE
    initOgreAR(CameraParamsUnd, leftUnd.ptr<uchar>(0), "../data/ogre/");

    while (true)
    {
        orsens.grabSensorData();

        vector<Marker> left_markers = orsens.getMarkers(0);

        left = orsens.getLeft();
        undistort(left,leftUnd,orsens.getARCameraParametres(0).CameraMatrix,orsens.getARCameraParametres(0).Distorsion);

        /// UPDATE BACKGROUND IMAGE
        mTexture->getBuffer()->blitFromMemory(mPixelBox);

        /// UPDATE SCENE
        int j;
        double position[3], orientation[4];
        ogreNode[0]->setVisible(false);
        ogreNode[1]->setVisible(false);
        ogreNode[2]->setVisible(false);
        // show nodes for detected markers
        int index=3;
        for(j=0; j<left_markers.size() /*&& j<MAX_MARKERS*/; j++)
        {
            if(left_markers[j].id==461) {
                left_markers[j].OgreGetPoseParameters(position, orientation);
                ogreNode[0]->setPosition( position[0], position[1], position[2]  );
                ogreNode[0]->setOrientation( orientation[0], orientation[1], orientation[2], orientation[3]  );
                ogreNode[0]->setVisible(true);
            }
            else if(left_markers[j].id==233) {
                left_markers[j].OgreGetPoseParameters(position, orientation);
                ogreNode[1]->setPosition( position[0], position[1], position[2]  );
                ogreNode[1]->setOrientation( orientation[0], orientation[1], orientation[2], orientation[3]  );
                ogreNode[1]->setVisible(true);
            }
            else if(left_markers[j].id==684) {
                left_markers[j].OgreGetPoseParameters(position, orientation);
                ogreNode[2]->setPosition( position[0], position[1], position[2]  );
                ogreNode[2]->setOrientation( orientation[0], orientation[1], orientation[2], orientation[3]  );
                ogreNode[2]->setVisible(true);
            }
            else if(index<MAX_MARKERS) {
                left_markers[j].OgreGetPoseParameters(position, orientation);
                ogreNode[index]->setPosition( position[0], position[1], position[2]  );
                ogreNode[index]->setOrientation( orientation[0], orientation[1], orientation[2], orientation[3]  );
                ogreNode[index]->setVisible(true);
                index++;
            }
        }
        // hide rest of nodes
        for( ; index<MAX_MARKERS; index++) ogreNode[index]->setVisible(false);

        // Update animation
        double deltaTime = 1.2*root->getTimer()->getMilliseconds()/1000.;
        for(int i=0; i<MAX_MARKERS; i++)
        {
	        baseAnim[i]->addTime(deltaTime);
	        topAnim[i]->addTime(deltaTime);
        }
        root->getTimer()->reset();

        /// RENDER FRAME
        if(root->renderOneFrame() == false) break;
        Ogre::WindowEventUtilities::messagePump();

        /// KEYBOARD INPUT
        keyboard->capture();
        if (keyboard->isKeyDown(OIS::KC_ESCAPE)) break;

        //imshow("left", left);

        /*if (mode==1)
        {
            cout<<endl << "right ";
            vector<Marker> right_markers = orsens.getMarkers(1);

            Mat right = orsens.getRight();

            for (unsigned int i=0; i<right_markers.size(); i++)
            {
                cout<<endl<<right_markers[i];
                Mat Rmat_right;
                cv::Rodrigues(right_markers[i].Rvec, Rmat_right);
                cout<<endl << Rmat_right;

                right_markers[i].draw(right,Scalar(0,0,255),1);

                if (orsens.getARCameraParametres(1).isValid())
                {
                    CvDrawingUtils::draw3dCube(right,right_markers[i],orsens.getARCameraParametres(1));
                    CvDrawingUtils::draw3dAxis(right,right_markers[i],orsens.getARCameraParametres(1));
                }
            }

            imshow("right", right);

        }*/

        //char c = waitKey(1000/orsens.getRate());

        //if (c==27)
          //  break;


    }

    orsens.deinitAR();

    orsens.stop();

    im->destroyInputObject(keyboard);
    im->destroyInputSystem(im);
    im = 0;

    delete root;

    return 0;
}
