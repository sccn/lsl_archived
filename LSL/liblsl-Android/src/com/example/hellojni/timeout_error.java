/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.9
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.example.hellojni;

public class timeout_error {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected timeout_error(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(timeout_error obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        lslAndroidJNI.delete_timeout_error(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public timeout_error(String msg) {
    this(lslAndroidJNI.new_timeout_error(msg), true);
  }

}
