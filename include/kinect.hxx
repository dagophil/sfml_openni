#ifndef KINECT_HXX
#define KINECT_HXX

#include <stdexcept>
#include <string>
#include <vector>
#include <ostream>
#include <map>
#include <thread>

#include <ni/XnOS.h>
#include <ni/XnCppWrapper.h>

#include "ndarray.hxx"
#include "utility.hxx"



std::ostream & operator<<(std::ostream & s, XnPoint3D const & p)
{
    s << "(" << p.X << ", " << p.Y << ", " << p.Z << ")";
    return s;
}



namespace kin
{

/**
 * @brief Static constant array with all possible joints.
 */
static std::array<XnSkeletonJoint, 24> const skeleton_joints {
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
    XnLabel id_;
    bool visible_;
    std::map<XnSkeletonJoint, JointInfo> joints_;

    explicit User(XnLabel id = 0, bool visible = true)
        :
          id_(id),
          visible_(visible)
    {}
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
    UpdateDetails update();

    /**
     * @brief Return the depth data.
     */
    Array2D<XnDepthPixel> const & depth_data() const
    {
        return depth_data_;
    }

    /**
     * @brief Return the user data.
     */
    Array2D<XnLabel> const & user_data() const
    {
        return user_data_;
    }

private:

    /**
     * @brief Check if the status is OK and throw an exception if it is not.
     */
    void check_error(XnStatus status);

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

    xn::Context context_; // the kinect context

    xn::DepthGenerator depth_generator_; // the depth generator
    xn::DepthMetaData depth_meta_; // the depth meta data
    Array2D<XnDepthPixel> depth_data_; // the actual depth data

    xn::UserGenerator user_generator_; // the user generator
    xn::SceneMetaData user_meta_; // the user meta data
    Array2D<XnLabel> user_data_; // the combined pixel data of all users
    std::vector<User> users_; // the current users
    std::vector<bool> user_visible_; // keeps track of the visibility of the users
};

KinectSensor::KinectSensor()
{
    // Initialize the kinect components.
    check_error(context_.Init());
    check_error(depth_generator_.Create(context_));
    check_error(user_generator_.Create(context_));
    if (!user_generator_.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
        throw std::runtime_error("User generator does not support skeleton.");
    context_.SetGlobalMirror(true);

    // Register the callbacks for the user generator.
    XnCallbackHandle hUser, hCalibrationStart, hCalibrationComplete, hUserExit, hUserReenter;
    check_error(user_generator_.RegisterUserCallbacks(user_new, user_lost, this, hUser));
    check_error(user_generator_.GetSkeletonCap().RegisterToCalibrationStart(user_calibration_start, this, hCalibrationStart));
    check_error(user_generator_.GetSkeletonCap().RegisterToCalibrationComplete(user_calibration_complete, this, hCalibrationComplete));
    check_error(user_generator_.RegisterToUserExit(user_exit, this, hUserExit));
    check_error(user_generator_.RegisterToUserReEnter(user_reenter, this, hUserReenter));
    user_generator_.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

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

UpdateDetails KinectSensor::update()
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
                auto & user = users_.back();
                for (auto j : skeleton_joints)
                {
                    if (!user_generator_.GetSkeletonCap().IsJointActive(j))
                        continue;
                    XnSkeletonJointPosition jpos;
                    user_generator_.GetSkeletonCap().GetSkeletonJointPosition(user_id, j, jpos);
                    if (jpos.fConfidence < 0.7)
                        continue;
                    XnPoint3D proj_pos;
                    depth_generator_.ConvertRealWorldToProjective(1, &jpos.position, &proj_pos);
                    user.joints_[j] = JointInfo(j, jpos.fConfidence, jpos.position, proj_pos);
                }
            }
        }
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
    else if (status == XN_CALIBRATION_STATUS_MANUAL_ABORT)
    {
        std::cout << "calibration aborted for user " << id << std::endl;
    }
    else
    {
        std::cout << "calibration failed for user " << id << ", trying again" << std::endl;
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



} // namespace kin



#endif
