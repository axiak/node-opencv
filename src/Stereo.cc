
#include "Stereo.h"

#if CV_MAJOR_VERSION >= 3
#ifdef __GNUC__
#warning TODO: port me to OpenCV 3
#else
// vs style message pragma
#pragma message ( "TODO: port me to OpenCV 3" )
#endif
#endif

#if CV_MAJOR_VERSION < 3
#include "Matrix.h"
#include <opencv2/legacy/legacy.hpp>

// Block matching

Nan::Persistent<FunctionTemplate> StereoBM::constructor;

void StereoBM::Init(Local<Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(StereoBM::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("StereoBM").ToLocalChecked());

  Nan::SetPrototypeMethod(ctor, "compute", Compute);

  ctor->Set(Nan::New<String>("BASIC_PRESET").ToLocalChecked(), Nan::New<Integer>((int)cv::StereoBM::BASIC_PRESET));
  ctor->Set(Nan::New<String>("FISH_EYE_PRESET").ToLocalChecked(), Nan::New<Integer>((int)cv::StereoBM::FISH_EYE_PRESET));
  ctor->Set(Nan::New<String>("NARROW_PRESET").ToLocalChecked(), Nan::New<Integer>((int)cv::StereoBM::NARROW_PRESET));

  target->Set(Nan::GetCurrentContext(),Nan::New("StereoBM").ToLocalChecked(), ctor->GetFunction( Nan::GetCurrentContext() ).ToLocalChecked());
}

NAN_METHOD(StereoBM::New) {
  Nan::HandleScope scope;

  if (info.This()->InternalFieldCount() == 0) {
    Nan::ThrowTypeError("Cannot instantiate without new");
  }

  StereoBM *stereo;

  if (info.Length() == 0) {
    stereo = new StereoBM();
  } else if (info.Length() == 1) {
    // preset
    stereo = new StereoBM(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
  } else if (info.Length() == 2) {
    // preset, disparity search range
    stereo = new StereoBM(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[1]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
  } else {
    stereo = new StereoBM(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[1]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
        // preset, disparity search range, sum of absolute differences window size
        info[2]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
  }

  stereo->Wrap(info.Holder());
  info.GetReturnValue().Set(info.Holder());
}

StereoBM::StereoBM(int preset, int ndisparities, int SADWindowSize) :
    Nan::ObjectWrap(),
    stereo(preset, ndisparities, SADWindowSize) {
}

// TODO make this async
NAN_METHOD(StereoBM::Compute) {
  SETUP_FUNCTION(StereoBM)

  try {
    // Get the arguments

    // Arg 0, the 'left' image
    Matrix* m0 = Nan::ObjectWrap::Unwrap<Matrix>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    cv::Mat left = m0->mat;

    // Arg 1, the 'right' image
    Matrix* m1 = Nan::ObjectWrap::Unwrap<Matrix>(info[1]->ToObject(Nan::GetCurrentContext()).ToLocalChecked());
    cv::Mat right = m1->mat;

    // Optional 3rd arg, the disparty depth
    int type = CV_16S;
    if (info.Length() > 2) {
      type = info[2]->IntegerValue( Nan::GetCurrentContext() ).ToChecked();
    }

    // Compute stereo using the block matching algorithm
    cv::Mat disparity;
    self->stereo(left, right, disparity, type);

    // Wrap the returned disparity map
    Local < Object > disparityWrap = Matrix::CreateWrappedFromMat(disparity);

    info.GetReturnValue().Set(disparityWrap);

  } catch (cv::Exception &e) {
    const char *err_msg = e.what();
    Nan::ThrowError(err_msg);
    return;
  }
}

// Semi-Global Block matching
Nan::Persistent<FunctionTemplate> StereoSGBM::constructor;

void StereoSGBM::Init(Local<Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(StereoSGBM::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("StereoSGBM").ToLocalChecked());

  Nan::SetPrototypeMethod(ctor, "compute", Compute);

  target->Set(Nan::GetCurrentContext(), Nan::New("StereoSGBM").ToLocalChecked(), ctor->GetFunction( Nan::GetCurrentContext() ).ToLocalChecked());
}

NAN_METHOD(StereoSGBM::New) {
  Nan::HandleScope scope;

  if (info.This()->InternalFieldCount() == 0) {
    Nan::ThrowTypeError("Cannot instantiate without new");
  }

  StereoSGBM *stereo;

  if (info.Length() == 0) {
    stereo = new StereoSGBM();
  } else {
    // If passing arguments, must pass the first 3 at least
    if (info.Length() >= 3) {
      switch (info.Length()) {
        case 3:
        stereo = new StereoSGBM(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[1]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[2]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
        break;
        case 4:
        stereo = new StereoSGBM(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[1]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[2]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[3]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
        break;
        case 5:
        stereo = new StereoSGBM(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[1]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[2]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[3]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[4]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
        break;
        case 6:
        stereo = new StereoSGBM(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[1]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[2]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[3]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[4]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[5]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
        break;
        case 7:
        stereo = new StereoSGBM(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[1]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[2]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[3]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[4]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[5]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[6]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
        break;
        case 8:
        stereo = new StereoSGBM(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[1]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[2]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[3]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[4]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[5]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[6]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[7]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
        break;
        case 9:
        stereo = new StereoSGBM(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[1]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[2]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[3]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[4]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[5]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[6]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[7]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[8]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
        break;
        case 10:
        stereo = new StereoSGBM(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[1]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[2]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[3]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[4]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[5]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[6]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[7]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[8]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[9]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
        break;
        default:
        stereo = new StereoSGBM(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[1]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[2]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[3]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[4]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[5]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[6]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[7]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(),
            info[8]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[9]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[10]->ToBoolean( v8::Isolate::GetCurrent() )->Value());
        break;
      }
    } else {
      Nan::ThrowError("If overriding default settings, must pass minDisparity, numDisparities, and SADWindowSize");
      return;
    }
  }

  stereo->Wrap(info.Holder());
  info.GetReturnValue().Set(info.Holder());
}

StereoSGBM::StereoSGBM() :
    Nan::ObjectWrap(),
    stereo() {

}

StereoSGBM::StereoSGBM(int minDisparity, int ndisparities, int SADWindowSize,
    int p1, int p2, int disp12MaxDiff, int preFilterCap, int uniquenessRatio,
    int speckleWindowSize, int speckleRange, bool fullDP) :
    Nan::ObjectWrap(),
    stereo(minDisparity, ndisparities, SADWindowSize, p1, p2, disp12MaxDiff,
        preFilterCap, uniquenessRatio, speckleWindowSize, speckleRange, fullDP) {
}

// TODO make this async
NAN_METHOD(StereoSGBM::Compute) {
  SETUP_FUNCTION(StereoSGBM)

  try {
    // Get the arguments

    // Arg 0, the 'left' image
    Matrix* m0 = Nan::ObjectWrap::Unwrap<Matrix>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    cv::Mat left = m0->mat;

    // Arg 1, the 'right' image
    Matrix* m1 = Nan::ObjectWrap::Unwrap<Matrix>(info[1]->ToObject(Nan::GetCurrentContext()).ToLocalChecked());
    cv::Mat right = m1->mat;

    // Compute stereo using the block matching algorithm
    cv::Mat disparity;
    self->stereo(left, right, disparity);

    // Wrap the returned disparity map
    Local < Object > disparityWrap = Matrix::CreateWrappedFromMat(disparity);

    info.GetReturnValue().Set(disparityWrap);
  } catch (cv::Exception &e) {
    const char *err_msg = e.what();
    Nan::ThrowError(err_msg);
    return;
  }
}

// Graph cut

Nan::Persistent<FunctionTemplate> StereoGC::constructor;

void StereoGC::Init(Local<Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(StereoGC::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("StereoGC").ToLocalChecked());

  Nan::SetPrototypeMethod(ctor, "compute", Compute);

  target->Set(Nan::GetCurrentContext(),Nan::New("StereoGC").ToLocalChecked(), ctor->GetFunction( Nan::GetCurrentContext() ).ToLocalChecked());
}

NAN_METHOD(StereoGC::New) {
  Nan::HandleScope scope;

  if (info.This()->InternalFieldCount() == 0)
  Nan::ThrowTypeError("Cannot instantiate without new");

  StereoGC *stereo;

  if (info.Length() == 0) {
    stereo = new StereoGC();
  } else if (info.Length() == 1) {
    // numberOfDisparities
    stereo = new StereoGC(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
  } else {
    // max iterations
    stereo = new StereoGC(info[0]->IntegerValue( Nan::GetCurrentContext() ).ToChecked(), info[1]->IntegerValue( Nan::GetCurrentContext() ).ToChecked());
  }

  stereo->Wrap(info.Holder());
  info.GetReturnValue().Set(info.Holder());
}

StereoGC::StereoGC(int numberOfDisparities, int maxIters) :
    Nan::ObjectWrap() {
  stereo = cvCreateStereoGCState(numberOfDisparities, maxIters);
}

// TODO make this async
NAN_METHOD(StereoGC::Compute) {
  SETUP_FUNCTION(StereoGC)

  try {
    // Get the arguments

    // Arg 0, the 'left' image
    Matrix* m0 = Nan::ObjectWrap::Unwrap<Matrix>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
    cv::Mat left = m0->mat;

    // Arg 1, the 'right' image
    Matrix* m1 = Nan::ObjectWrap::Unwrap<Matrix>(info[1]->ToObject(Nan::GetCurrentContext()).ToLocalChecked());
    cv::Mat right = m1->mat;

    // Compute stereo using the block matching algorithm
    CvMat left_leg = left, right_leg = right;
    CvMat *disp_left = cvCreateMat(left.rows, left.cols, CV_16S), *disp_right =
        cvCreateMat(right.rows, right.cols, CV_16S);
    cvFindStereoCorrespondenceGC(&left_leg, &right_leg, disp_left, disp_right,
        self->stereo, 0);

    cv::Mat disp16 = disp_left;
    cv::Mat disparity(disp16.rows, disp16.cols, CV_8U);
    disp16.convertTo(disparity, CV_8U, -16);

    // Wrap the returned disparity map
    Local < Object > disparityWrap = Matrix::CreateWrappedFromMat(disparity);

    info.GetReturnValue().Set(disparityWrap);
  } catch (cv::Exception &e) {
    const char *err_msg = e.what();
    Nan::ThrowError(err_msg);
    return;
  }
}

#endif
