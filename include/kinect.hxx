#ifndef KINECT_HXX
#define KINECT_HXX

#include <stdexcept>
#include <string>
#include <vector>
#include <ostream>
#include <map>
#include <array>

#include "platform_support.hxx"
#include <XnCppWrapper.h>

#include "ndarray.hxx"
#include "utility.hxx"


namespace kin
{

/**
 * @brief Static constant array with all possible joints.
 */
static std::array<XnSkeletonJoint, 24> const skeleton_joints = {
    XN_SKEL_HEAD,
    XN_SKEL_NECK,
    XN_SKEL_TORSO,
    XN_SKEL_WAIST,
    XN_SKEL_LEFT_COLLAR,
    XN_SKEL_LEFT_SHOULDER,
    XN_SKEL_LEFT_ELBOW,
    XN_SKEL_LEFT_WRIST,
    XN_SKEL_LEFT_HAND,
    XN_SKEL_LEFT_FINGERTIP,
    XN_SKEL_RIGHT_COLLAR,
    XN_SKEL_RIGHT_SHOULDER,
    XN_SKEL_RIGHT_ELBOW,
    XN_SKEL_RIGHT_WRIST,
    XN_SKEL_RIGHT_HAND,
    XN_SKEL_RIGHT_FINGERTIP,
    XN_SKEL_LEFT_HIP,
    XN_SKEL_LEFT_KNEE,
    XN_SKEL_LEFT_ANKLE,
    XN_SKEL_LEFT_FOOT,
    XN_SKEL_RIGHT_HIP,
    XN_SKEL_RIGHT_KNEE,
    XN_SKEL_RIGHT_ANKLE,
    XN_SKEL_RIGHT_FOOT
};

/**
 * @brief The JointInfo struct contains position and confidence values about a single joint.
 */
struct JointInfo
{
    XnSkeletonJoint joint_;
    XnConfidence confidence_;
    XnPoint3D real_position_;
    XnPoint3D proj_position_;

    explicit JointInfo(XnSkeletonJoint joint = XnSkeletonJoint(),
                       XnConfidence confidence = 0,
                       XnPoint3D const & real_position = XnPoint3D(),
                       XnPoint3D const & proj_position = XnPoint3D())
        :
          joint_(joint),
          confidence_(confidence),
          real_position_(real_position),
          proj_position_(proj_position)
    {}
};

/**
 * @brief The User class stores user information, e. g. whether the user left the scene.
 */
struct User
{
public:

    XnLabel id_;
    bool visible_;
    std::map<XnSkeletonJoint, JointInfo> joints_;

    explicit User(XnLabel id = 0, bool visible = true)
        :
          id_(id),
          visible_(visible)
    {
        base_change_ = sf::Matrix3::Identity;
    }

    /**
     * @brief Compute the base change matrix (real coordinates -> user plane coordinates).
     */
    void compute_base_change()
    {
        if (joints_.count(XN_SKEL_LEFT_SHOULDER) > 0 &&
            joints_.count(XN_SKEL_RIGHT_SHOULDER) > 0)
        {
            auto s0 = joints_.at(XN_SKEL_LEFT_SHOULDER).real_position_;
            auto s1 = joints_.at(XN_SKEL_RIGHT_SHOULDER).real_position_;
            auto len = length(s1 - s0);

            sf::Matrix3 base(s1.X - s0.X, 0, s1.Z - s0.Z, 0, 1, 0, s1.Z - s0.Z, 0, s0.X - s1.X);
            base_change_ = base.GetInverse();
            if (len > 0)
            {
                base_change_(1, 0) /= len;
                base_change_(1, 1) /= len;
                base_change_(1, 2) /= len;
            }
        }
    }

    /**
     * @brief Transform the given vector from real coordinates to user plane coordinates.
     */
    XnVector3D transform_vector(XnVector3D const & v) const
    {
        XnVector3D ret;
        ret.X = base_change_(0, 0) * v.X + base_change_(0, 1) * v.Y + base_change_(0, 2) * v.Z;
        ret.Y = base_change_(1, 0) * v.X + base_change_(1, 1) * v.Y + base_change_(1, 2) * v.Z;
        ret.Z = base_change_(2, 0) * v.X + base_change_(2, 1) * v.Y + base_change_(2, 2) * v.Z;
        return ret;
    }

private:

    sf::Matrix3 base_change_; // the base change matrix

};

/**
 * @brief The UpdateDetails struct can be used to see which generators where updated in a kinect update step.
 */
struct UpdateDetails
{
    explicit UpdateDetails(bool depth = false, bool user = false)
        :
          depth_(depth),
          user_(user)
    {}
    bool depth_;
    bool user_;
};

/**
 * @brief The KinectSensor class runs a thread to gather data from a kinect sensor.
 */
class KinectSensor
{
public:

    /**
     * @brief Initialize the kinect components and start the thread that generates the depth data.
     */
    KinectSensor();

    /**
     * @brief Release the kinect components and join the thread.
     */
    ~KinectSensor();

    /**
     * @brief Disable copy constructor to avoid duplicate kinect access.
     */
    KinectSensor(KinectSensor const & other) = delete;

    /**
     * @brief Disable assignment operator to avoid duplicate kinect access.
     */
    KinectSensor & operator=(KinectSensor const & other) = delete;

    /**
     * @brief Return the x resolution of the depth generator.
     * @return the x resolution
     */
    XnUInt32 x_res() const;

    /**
     * @brief Return the y resolution of the depth generator.
     * @return the y resolution
     */
    XnUInt32 y_res() const;

    /**
     * @brief Return the z resolution of the depth generator.
     * @return the z resolution
     */
    XnUInt32 z_res() const;

    /**
     * @brief The update method checks if the generators have produced new data. It should be called once per frame.
     */
    UpdateDetails update(float elapsed_time);

    /**
     * @brief Return the depth data.
     */
    Array2D<XnDepthPixel> const & depth_data() const
    {
        return depth_data_;
    }

    /**
     * @brief Return the user pixel data.
     */
    Array2D<XnLabel> const & user_data() const
    {
        return user_data_;
    }

    /**
     * @brief Return the users.
     */
    std::vector<User> const & users() const
    {
        return users_;
    }
    
    /**
     * @brief Return the position of the left hand.
     */
    XnVector3D hand_left() const
    {
        auto p = hand_left_.mean();
        p.X = (p.X + 1.5) / 1.75;
        p.Y = (p.Y - 0.7) / 1.5;
//        p.Z = (p.Z + 1.0);
        return p;
    }
    
    /**
     * @brief Return the position of the right hand.
     */
    XnVector3D hand_right() const
    {
        auto p = hand_right_.mean();
        p.X = (p.X + 0.25) / 1.75;
        p.Y = (p.Y - 0.7) / 1.5;
//        p.Z = (p.Z + 1.0);
        return p;
    }

    /**
     * @brief Return whether the left hand is visible.
     */
    bool hand_left_visible() const
    {
        return hand_left_visible_;
    }

    /**
     * @brief Return whether the right hand is visible.
     */
    bool hand_right_visible() const
    {
        return hand_right_visible_;
    }

    /**
     * @brief Callback for clicks with the left hand.
     */
    std::function<void()> & handle_click_left()
    {
        return click_detector_left_.handle_click_;
    }

    /**
     * @brief Callback for clicks with the right hand.
     */
    std::function<void()> & handle_click_right()
    {
        return click_detector_right_.handle_click_;
    }

    /**
     * @brief Use depth for click detection.
     */
    void use_y_click()
    {
        click_detector_left_.use_y_ = true;
        click_detector_right_.use_y_ = true;
    }

    /**
     * @brief Use height for click detection.
     */
    void use_z_click()
    {
        click_detector_left_.use_y_ = false;
        click_detector_right_.use_y_ = false;
    }

private:

    /**
     * @brief Check if the status is OK and throw an exception if it is not.
     */
    void check_error(XnStatus status);
    
    /**
     * @brief Compute the hand positions.
     */
    void compute_hand_positions();

    /**
     * @brief Check if the user made a click gesture.
     */
    void check_for_clicks(float elapsed_time);

    /**
     * @brief Callback for the "new user" event. Starts the calibration.
     * @note The callbacks are static functions because the register method takes function pointers and class member functions cannot be converted to function pointers.
     */
    static void XN_CALLBACK_TYPE user_new(xn::UserGenerator & gen, XnUserID id, void* sensor_ptr);

    /**
     * @brief Callback for the "lost user" event.
     */
    static void XN_CALLBACK_TYPE user_lost(xn::UserGenerator & gen, XnUserID id, void* sensor_ptr);

    /**
     * @brief Callback for the "start calibration" event.
     */
    static void XN_CALLBACK_TYPE user_calibration_start(xn::SkeletonCapability & cap, XnUserID id, void* sensor_ptr);

    /**
     * @brief Callback for the "calibration complete" event. Starts user tracking.
     */
    static void XN_CALLBACK_TYPE user_calibration_complete(xn::SkeletonCapability & cap, XnUserID id, XnCalibrationStatus status, void* sensor_ptr);

    /**
     * @brief Callback for the "user left scene" event.
     */
    static void XN_CALLBACK_TYPE user_exit(xn::UserGenerator & gen, XnUserID id, void* sensor_ptr);

    /**
     * @brief Callback for the "user reentered scene" event.
     */
    static void XN_CALLBACK_TYPE user_reenter(xn::UserGenerator & gen, XnUserID id, void* sensor_ptr);

    /**
     * @brief Callback for "pose detected" Event
     */
    static void XN_CALLBACK_TYPE pose_detected(xn::PoseDetectionCapability& , const XnChar* strPose, XnUserID nId, void*);

//    static void XN_CALLBACK_TYPE gesture_recognized(xn::GestureGenerator &, const XnChar *strGesture, const XnPoint3D *pIDPosition, const XnPoint3D *pEndPosition, void *sensor_ptr);

//    static void XN_CALLBACK_TYPE gesture_progress(xn::GestureGenerator &generator, const XnChar *strGesture, const XnPoint3D *pPosition, XnFloat fProgress, void *pCookie);

    xn::Context context_; // the kinect context

    xn::DepthGenerator depth_generator_; // the depth generator
    xn::DepthMetaData depth_meta_; // the depth meta data
    Array2D<XnDepthPixel> depth_data_; // the actual depth data

    xn::UserGenerator user_generator_; // the user generator
    xn::SceneMetaData user_meta_; // the user meta data
    bool need_pose_; // required pose for calibration
    std::string pose_name_;
    char* pose_name_ptr_;
    Array2D<XnLabel> user_data_; // the combined pixel data of all users
    std::vector<User> users_; // the current users
    std::vector<bool> user_visible_; // keeps track of the visibility of the users

//    xn::GestureGenerator gesture_generator_; // the gesture generator
//    XnBoundingBox3D* bounding_box_; // the gesture bounding box

    Averager<XnVector3D, 10> hand_left_; // track the left hand
    Averager<XnVector3D, 10> hand_right_; // track the right hand
    bool hand_left_visible_; // whether the left hand is visible
    bool hand_right_visible_; // whether the right hand is visible

    ClickDetector click_detector_left_; // click detector for the left hand
    ClickDetector click_detector_right_; // click detector for the right hand

};

KinectSensor::KinectSensor()
    :
      need_pose_(false),
      pose_name_(20, ' '),
      pose_name_ptr_(&pose_name_[0]),
      hand_left_({0, 0, 0}),
      hand_right_({0, 0, 0}),
      hand_left_visible_(false),
      hand_right_visible_(false)
{
    // Initialize the kinect components.
    check_error(context_.Init());resource_manager
    check_error(depth_generator_.Create(context_));
    check_error(user_generator_.Create(context_));
    if (!user_generator_.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
        throw std::runtime_error("KinectSensor::KinectSensor(): User generator does not support skeleton.");
    context_.SetGlobalMirror(true);
//    check_error(gesture_generator_.Create(context_));

    // Register the callbacks for the user generator.
    XnCallbackHandle hUser, hCalibrationStart, hCalibrationComplete, hUserExit, hUserReenter, h_pose_detected,h_gesture;
    check_error(user_generator_.RegisterUserCallbacks(user_new, user_lost, this, hUser));
    check_error(user_generator_.GetSkeletonCap().RegisterToCalibrationStart(user_calibration_start, this, hCalibrationStart));
    check_error(user_generator_.GetSkeletonCap().RegisterToCalibrationComplete(user_calibration_complete, this, hCalibrationComplete));
    check_error(user_generator_.RegisterToUserExit(user_exit, this, hUserExit));
    check_error(user_generator_.RegisterToUserReEnter(user_reenter, this, hUserReenter));
    user_generator_.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

    // Check if the pose is needed for calibration.
    if (user_generator_.GetSkeletonCap().NeedPoseForCalibration())
    {
        need_pose_ = true;
        if (!user_generator_.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)){
            throw std::runtime_error("KinectSensor::KinectSensor(): User generator needs pose for calibration, "
                                     "but the kinect doesn't support it.");
        }
        check_error(user_generator_.GetPoseDetectionCap().RegisterToPoseDetected(pose_detected,this,h_pose_detected));
        user_generator_.GetSkeletonCap().GetCalibrationPose(pose_name_ptr_);
        //check_error(user_generator_.GetPoseDetectionCap().RegisterToPoseInProgress(pose_in_progress,this,h_pose_in_progress));
    }

//    // Register click gesture callbacks.
//    if (!gesture_generator_.IsGestureAvailable("Click"))
//        throw std::runtime_error("Click gesture not available.");
//    check_error(gesture_generator_.RegisterGestureCallbacks(gesture_recognized, gesture_progress, this, h_gesture));
//    check_error(gesture_generator_.AddGesture("Click", bounding_box_));

    // Start generating the kinect data.
    check_error(context_.StartGeneratingAll());

    // Get the kinect resolution to initialize the depth arrays.
    depth_generator_.GetMetaData(depth_meta_);
    depth_data_.resize(x_res(), y_res());
    user_data_.resize(x_res(), y_res());
}

KinectSensor::~KinectSensor()
{
    user_generator_.Release();
    depth_generator_.Release();
    context_.Release();
}

XnUInt32 KinectSensor::x_res() const
{
    return depth_meta_.XRes();
}

XnUInt32 KinectSensor::y_res() const
{
    return depth_meta_.YRes();
}

XnUInt32 KinectSensor::z_res() const
{
    return depth_meta_.ZRes();
}

UpdateDetails KinectSensor::update(float elapsed_time)
{
    UpdateDetails updates;

    if (depth_generator_.IsNewDataAvailable())
    {
        check_error(depth_generator_.WaitAndUpdateData());
        updates.depth_ = true;

        // Get the depth map.
        depth_generator_.GetMetaData(depth_meta_);
        for (size_t y = 0; y < y_res(); ++y)
            for (size_t x = 0; x < x_res(); ++x)
                depth_data_(x, y) = depth_meta_(x, y);
    }

    if (user_generator_.IsNewDataAvailable())
    {
        check_error(user_generator_.WaitAndUpdateData());
        updates.user_ = true;

        // Get the user pixels.
        user_generator_.GetUserPixels(0, user_meta_);
        for (size_t y = 0; y < y_res(); ++y)
            for (size_t x = 0; x < x_res(); ++x)
                user_data_(x, y) = user_meta_(x, y);

        // Get the user joints.
        std::vector<XnUserID> user_ids(user_visible_.size());
        XnUInt16 n_users = user_ids.size();
        user_generator_.GetUsers(&user_ids.front(), n_users);
        users_.clear();
        for (size_t i = 0; i < n_users; ++i)
        {
            auto const user_id = user_ids[i];
            if (user_generator_.GetSkeletonCap().IsTracking(user_id))
            {
                users_.emplace_back(user_id, user_visible_[user_id]);
                for (auto const j : skeleton_joints)
                {
                    if (!user_generator_.GetSkeletonCap().IsJointActive(j))
                        continue;
                    XnSkeletonJointPosition jpos;
                    user_generator_.GetSkeletonCap().GetSkeletonJointPosition(user_id, j, jpos);
                    if (jpos.fConfidence < 0.7)
                        continue;
                    XnPoint3D proj_pos;
                    depth_generator_.ConvertRealWorldToProjective(1, &jpos.position, &proj_pos);
                    users_.back().joints_[j] = JointInfo(j, jpos.fConfidence, jpos.position, proj_pos);
                }
                users_.back().compute_base_change();
            }
        }

        // Compute the new hand coordinates.
        compute_hand_positions();

        // Check for clicks.
        check_for_clicks(elapsed_time);
    }

    return updates;
}

void KinectSensor::check_error(XnStatus status)
{
    if (status != XN_STATUS_OK)
    {
        throw std::runtime_error("Kinect error: " + std::string(xnGetStatusString(status)));
    }
}

void XN_CALLBACK_TYPE KinectSensor::user_new(xn::UserGenerator & gen, XnUserID id, void* sensor_ptr)
{
    KinectSensor & k = *static_cast<KinectSensor*>(sensor_ptr);
    std::cout << "new user: " << id << std::endl;
    if (id+1 > k.user_visible_.size())
        k.user_visible_.resize(id+1, false);
    k.user_visible_[id] = true;
    if (k.need_pose_)
        k.user_generator_.GetPoseDetectionCap().StartPoseDetection(k.pose_name_ptr_, id);
    else
        k.user_generator_.GetSkeletonCap().RequestCalibration(id, TRUE);
}

void XN_CALLBACK_TYPE KinectSensor::user_lost(xn::UserGenerator & gen, XnUserID id, void* sensor_ptr)
{
    KinectSensor & k = *static_cast<KinectSensor*>(sensor_ptr);
    std::cout << "lost user " << id << std::endl;
    k.user_visible_[id] = false;
}

void XN_CALLBACK_TYPE KinectSensor::user_calibration_start(xn::SkeletonCapability & cap, XnUserID id, void* sensor_ptr)
{
    KinectSensor & k = *static_cast<KinectSensor*>(sensor_ptr);
    std::cout << "starting calibration user " << id << std::endl;
}

void XN_CALLBACK_TYPE KinectSensor::user_calibration_complete(xn::SkeletonCapability & cap, XnUserID id, XnCalibrationStatus status, void* sensor_ptr)
{
    KinectSensor & k = *static_cast<KinectSensor*>(sensor_ptr);
    if (status == XN_CALIBRATION_STATUS_OK)
    {
        std::cout << "calibration complete, start tracking user " << id << std::endl;
        k.user_generator_.GetSkeletonCap().StartTracking(id);
    }
    else
    {
        std::cout << "calibration failed for user " << id << ", trying again" << std::endl;
        if (k.need_pose_)
            k.user_generator_.GetPoseDetectionCap().StartPoseDetection(k.pose_name_ptr_, id);
        else
            k.user_generator_.GetSkeletonCap().RequestCalibration(id, TRUE);
    }
}

void XN_CALLBACK_TYPE KinectSensor::user_exit(xn::UserGenerator & gen, XnUserID id, void* sensor_ptr)
{
    KinectSensor & k = *static_cast<KinectSensor*>(sensor_ptr);
    std::cout << "user " << id << " left scene" << std::endl;
    k.user_visible_[id] = false;
}

void XN_CALLBACK_TYPE KinectSensor::user_reenter(xn::UserGenerator & gen, XnUserID id, void* sensor_ptr)
{
    KinectSensor & k = *static_cast<KinectSensor*>(sensor_ptr);
    std::cout << "user " << id << " reentered scene" << std::endl;
    k.user_visible_[id] = true;
}

void XN_CALLBACK_TYPE KinectSensor::pose_detected(xn::PoseDetectionCapability& , const XnChar* strPose, XnUserID nId, void* sensor_ptr)
{
    KinectSensor & k = *static_cast<KinectSensor*>(sensor_ptr);
    std::cout << "pose "<< strPose << " detected."<< std::endl;
    k.user_generator_.GetPoseDetectionCap().StopPoseDetection(nId);
    k.user_generator_.GetSkeletonCap().RequestCalibration(nId,true);
}

//void XN_CALLBACK_TYPE KinectSensor::gesture_recognized(xn::GestureGenerator &, const XnChar *strGesture, const XnPoint3D *pIDPosition, const XnPoint3D *pEndPosition, void *sensor_ptr)
//{
//    event_manager.post(Event(Event::KinectClick));
//}

//void XN_CALLBACK_TYPE KinectSensor::gesture_progress(xn::GestureGenerator &generator, const XnChar *strGesture, const XnPoint3D *pPosition, XnFloat fProgress, void *pCookie)
//{
//}

void KinectSensor::compute_hand_positions()
{
    // Only track if there are users.
    if (users_.size() == 0)
        return;
    
    // Only track if the main joints are known.
    auto const & u = users_.front();
    if (u.joints_.count(XN_SKEL_TORSO) == 0 ||
        u.joints_.count(XN_SKEL_LEFT_SHOULDER) == 0 ||
        u.joints_.count(XN_SKEL_RIGHT_SHOULDER) == 0)
        return;
        
    // Track the left hand.
    hand_left_visible_ = false;
    if (u.joints_.count(XN_SKEL_LEFT_HAND) > 0)
    {
        hand_left_visible_ = true;

        // Get the hand coordinates and transform them relative to the user plane position.
        auto hand_pos = u.joints_.at(XN_SKEL_LEFT_HAND).real_position_;
        auto torso = u.joints_.at(XN_SKEL_TORSO).real_position_;
        auto ret = u.transform_vector(hand_pos - torso);
        ret.Y = 1.5 - ret.Y;
        
        // Update the hand positions.
        if (hand_left_.empty())
            hand_left_.push(ret);
        else if (length(hand_left_.mean() - ret) > 0.04) // Stabilization: Only update if the hand moved a significant amount.
            hand_left_.push(ret);
    }
    
    // Track the right hand.
    hand_right_visible_ = false;
    if (u.joints_.count(XN_SKEL_RIGHT_HAND) > 0)
    {
        hand_right_visible_ = true;

        // Get the hand coordinates and transform them relative to the user plane position.
        auto hand_pos = u.joints_.at(XN_SKEL_RIGHT_HAND).real_position_;
        auto torso = u.joints_.at(XN_SKEL_TORSO).real_position_;
        auto ret = u.transform_vector(hand_pos - torso);
        ret.Y = 1.5 - ret.Y;
        
        // Update the hand positions.
        if (hand_right_.empty())
            hand_right_.push(ret);
        else if (length(hand_right_.mean() - ret) > 0.04) // Stabilization: Only update if the hand moved a significant amount.
            hand_right_.push(ret);
    }
}

void KinectSensor::check_for_clicks(float elapsed_time)
{
    if (hand_left_visible())
        click_detector_left_.update(elapsed_time, hand_left_.back());
    else
        click_detector_left_.reset();

    if (hand_right_visible())
        click_detector_right_.update(elapsed_time, hand_right_.back());
    else
        click_detector_left_.reset();
}



} // namespace kin



#endif
