#pragma once
#include <Arduino.h>
#include <math.h>

class Madgwick {
public:
    // Quaternion
    float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;

    float beta;              // algorithm gain
    float invSampleFreq;     // 1 / sample frequency

    Madgwick(float sampleFreq, float betaGain) {
        invSampleFreq = 1.0f / sampleFreq;
        beta = betaGain;
    }

    // =====================================================
    // IMU UPDATE (gyro + accel)
    // =====================================================
    void updateIMU(float gx, float gy, float gz,
                   float ax, float ay, float az) {

        float recipNorm;
        float s0, s1, s2, s3;
        float qDot1, qDot2, qDot3, qDot4;

        // Normalize accelerometer
        recipNorm = sqrt(ax*ax + ay*ay + az*az);
        if (recipNorm == 0.0f) return;
        recipNorm = 1.0f / recipNorm;
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Rate of change from gyro
        qDot1 = 0.5f * (-q1*gx - q2*gy - q3*gz);
        qDot2 = 0.5f * ( q0*gx + q2*gz - q3*gy);
        qDot3 = 0.5f * ( q0*gy - q1*gz + q3*gx);
        qDot4 = 0.5f * ( q0*gz + q1*gy - q2*gx);

        // Gradient descent correction
        s0 = 4*q0*q2*q2 + 2*q2*ax + 4*q0*q1*q1 - 2*q1*ay;
        s1 = 4*q1*q3*q3 - 2*q3*ax + 4*q0*q0*q1 - 2*q0*ay;
        s2 = 4*q0*q0*q2 + 2*q0*ax + 4*q2*q3*q3 - 2*q3*ay;
        s3 = 4*q1*q1*q3 - 2*q1*ax + 4*q2*q2*q3 - 2*q2*ay;

        recipNorm = 1.0f / sqrt(s0*s0 + s1*s1 + s2*s2 + s3*s3);
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Apply feedback
        qDot1 -= beta * s0;
        qDot2 -= beta * s1;
        qDot3 -= beta * s2;
        qDot4 -= beta * s3;

        // Integrate
        q0 += qDot1 * invSampleFreq;
        q1 += qDot2 * invSampleFreq;
        q2 += qDot3 * invSampleFreq;
        q3 += qDot4 * invSampleFreq;

        // Normalize quaternion
        recipNorm = 1.0f / sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
        q0 *= recipNorm;
        q1 *= recipNorm;
        q2 *= recipNorm;
        q3 *= recipNorm;
    }

    // =====================================================
    // AHRS UPDATE (gyro + accel + magnetometer)
    // =====================================================
    void update(float gx, float gy, float gz,
                float ax, float ay, float az,
                float mx, float my, float mz) {

        float recipNorm;
        float s0, s1, s2, s3;
        float qDot1, qDot2, qDot3, qDot4;
        float hx, hy, _2bx, _2bz;

        // Normalize accelerometer
        recipNorm = sqrt(ax*ax + ay*ay + az*az);
        if (recipNorm == 0.0f) return;
        recipNorm = 1.0f / recipNorm;
        ax *= recipNorm; ay *= recipNorm; az *= recipNorm;

        // Normalize magnetometer
        recipNorm = sqrt(mx*mx + my*my + mz*mz);
        if (recipNorm == 0.0f) return;
        recipNorm = 1.0f / recipNorm;
        mx *= recipNorm; my *= recipNorm; mz *= recipNorm;

        float _2q0 = 2.0f*q0;
        float _2q1 = 2.0f*q1;
        float _2q2 = 2.0f*q2;
        float _2q3 = 2.0f*q3;

        float q0q0 = q0*q0;
        float q1q1 = q1*q1;
        float q2q2 = q2*q2;
        float q3q3 = q3*q3;

        // Reference direction of Earth's magnetic field
        hx = mx*q0q0 - _2q0*my*q3 + _2q0*mz*q2 + mx*q1q1 +
             _2q1*my*q2 + _2q1*mz*q3 - mx*q2q2 - mx*q3q3;

        hy = _2q0*mx*q3 + my*q0q0 - _2q0*mz*q1 + _2q1*mx*q2 -
             my*q1q1 + my*q2q2 + _2q2*mz*q3 - my*q3q3;

        _2bx = sqrt(hx*hx + hy*hy);
        _2bz = -_2q0*mx*q2 + _2q0*my*q1 + mz*q0q0 +
               _2q1*mx*q3 - mz*q1q1 + _2q2*my*q3 - mz*q2q2 + mz*q3q3;

        // Gradient descent correction
        s0 = -_2q2*(2*(q1*q3 - q0*q2) - ax) +
              _2q1*(2*(q0*q1 + q2*q3) - ay) -
              _2bz*q2*(_2bx*(0.5f - q2q2 - q3q3) +
              _2bz*(q1*q3 - q0*q2) - mx);

        s1 =  _2q3*(2*(q1*q3 - q0*q2) - ax) +
              _2q0*(2*(q0*q1 + q2*q3) - ay) -
              4*q1*(1 - 2*(q1q1 + q2q2) - az);

        s2 = -_2q0*(2*(q1*q3 - q0*q2) - ax) +
              _2q3*(2*(q0*q1 + q2*q3) - ay) -
              4*q2*(1 - 2*(q1q1 + q2q2) - az);

        s3 =  _2q1*(2*(q1*q3 - q0*q2) - ax) +
              _2q2*(2*(q0*q1 + q2*q3) - ay);

        recipNorm = 1.0f / sqrt(s0*s0 + s1*s1 + s2*s2 + s3*s3);
        s0 *= recipNorm; s1 *= recipNorm;
        s2 *= recipNorm; s3 *= recipNorm;

        // Apply feedback
        qDot1 = 0.5f * (-q1*gx - q2*gy - q3*gz) - beta*s0;
        qDot2 = 0.5f * ( q0*gx + q2*gz - q3*gy) - beta*s1;
        qDot3 = 0.5f * ( q0*gy - q1*gz + q3*gx) - beta*s2;
        qDot4 = 0.5f * ( q0*gz + q1*gy - q2*gx) - beta*s3;

        q0 += qDot1 * invSampleFreq;
        q1 += qDot2 * invSampleFreq;
        q2 += qDot3 * invSampleFreq;
        q3 += qDot4 * invSampleFreq;

        recipNorm = 1.0f / sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
        q0 *= recipNorm; q1 *= recipNorm;
        q2 *= recipNorm; q3 *= recipNorm;
    }

    // =====================================================
    // Euler angles
    // =====================================================
    float getRoll() {
        return atan2(2*(q0*q1 + q2*q3),
                     1 - 2*(q1*q1 + q2*q2)) * RAD_TO_DEG;
    }

    float getPitch() {
        float v = 2*(q0*q2 - q3*q1);
        v = constrain(v, -1.0f, 1.0f);
        return asin(v) * RAD_TO_DEG;
    }

    float getYaw() {
        return atan2(2*(q0*q3 + q1*q2),
                     1 - 2*(q2*q2 + q3*q3)) * RAD_TO_DEG;
    }
};
