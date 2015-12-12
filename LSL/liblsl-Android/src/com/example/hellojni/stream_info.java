/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.1
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.example.hellojni;

public class stream_info {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected stream_info(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(stream_info obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        lslAndroidJNI.delete_stream_info(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public stream_info(String name, String type, int channel_count, double nominal_srate, channel_format_t channel_format, String source_id) {
    this(lslAndroidJNI.new_stream_info__SWIG_0(name, type, channel_count, nominal_srate, channel_format.swigValue(), source_id), true);
  }

  public stream_info(String name, String type, int channel_count, double nominal_srate, channel_format_t channel_format) {
    this(lslAndroidJNI.new_stream_info__SWIG_1(name, type, channel_count, nominal_srate, channel_format.swigValue()), true);
  }

  public stream_info(String name, String type, int channel_count, double nominal_srate) {
    this(lslAndroidJNI.new_stream_info__SWIG_2(name, type, channel_count, nominal_srate), true);
  }

  public stream_info(String name, String type, int channel_count) {
    this(lslAndroidJNI.new_stream_info__SWIG_3(name, type, channel_count), true);
  }

  public stream_info(String name, String type) {
    this(lslAndroidJNI.new_stream_info__SWIG_4(name, type), true);
  }

  public stream_info(SWIGTYPE_p_lsl_streaminfo handle) {
    this(lslAndroidJNI.new_stream_info__SWIG_5(SWIGTYPE_p_lsl_streaminfo.getCPtr(handle)), true);
  }

  public String name() {
    return lslAndroidJNI.stream_info_name(swigCPtr, this);
  }

  public String type() {
    return lslAndroidJNI.stream_info_type(swigCPtr, this);
  }

  public int channel_count() {
    return lslAndroidJNI.stream_info_channel_count(swigCPtr, this);
  }

  public double nominal_srate() {
    return lslAndroidJNI.stream_info_nominal_srate(swigCPtr, this);
  }

  public channel_format_t channel_format() {
    return channel_format_t.swigToEnum(lslAndroidJNI.stream_info_channel_format(swigCPtr, this));
  }

  public String source_id() {
    return lslAndroidJNI.stream_info_source_id(swigCPtr, this);
  }

  public int version() {
    return lslAndroidJNI.stream_info_version(swigCPtr, this);
  }

  public double created_at() {
    return lslAndroidJNI.stream_info_created_at(swigCPtr, this);
  }

  public String uid() {
    return lslAndroidJNI.stream_info_uid(swigCPtr, this);
  }

  public String session_id() {
    return lslAndroidJNI.stream_info_session_id(swigCPtr, this);
  }

  public String hostname() {
    return lslAndroidJNI.stream_info_hostname(swigCPtr, this);
  }

  public xml_element desc() {
    return new xml_element(lslAndroidJNI.stream_info_desc(swigCPtr, this), true);
  }

  public String as_xml() {
    return lslAndroidJNI.stream_info_as_xml(swigCPtr, this);
  }

  public int channel_bytes() {
    return lslAndroidJNI.stream_info_channel_bytes(swigCPtr, this);
  }

  public int sample_bytes() {
    return lslAndroidJNI.stream_info_sample_bytes(swigCPtr, this);
  }

  public SWIGTYPE_p_lsl_streaminfo handle() {
    return new SWIGTYPE_p_lsl_streaminfo(lslAndroidJNI.stream_info_handle(swigCPtr, this), true);
  }

  public stream_info() {
    this(lslAndroidJNI.new_stream_info__SWIG_6(), true);
  }

  public stream_info(stream_info rhs) {
    this(lslAndroidJNI.new_stream_info__SWIG_7(stream_info.getCPtr(rhs), rhs), true);
  }

  public static stream_info from_xml(String xml) {
    return new stream_info(lslAndroidJNI.stream_info_from_xml(xml), true);
  }

}
