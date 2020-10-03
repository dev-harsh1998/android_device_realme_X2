/*
 * Copyright (C) 2015 The CyanogenMod Project
 *               2017-2018 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.lineageos.settings.doze;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.os.SystemClock;
import android.util.Log;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class PickupSensor implements SensorEventListener {

    private static final boolean DEBUG = false;
    private static final String TAG = "PickupSensor";

    private static final int MIN_PULSE_INTERVAL_MS = 2500;
    private static final int WAKELOCK_TIMEOUT_MS = 300;

    private SensorManager mSensorManager;
    private Sensor mSensor;
    private Context mContext;
    private ExecutorService mExecutorService;
    private PowerManager mPowerManager;
    private WakeLock mWakeLock;

    private boolean doneDelay;

    private long mEntryTimestamp;

    public PickupSensor(Context context) {
        mContext = context;
        mSensorManager = mContext.getSystemService(SensorManager.class);
        mSensor = DozeUtils.getSensor(mSensorManager, "qti.sensor.amd");
        mPowerManager = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);
        mWakeLock = mPowerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, TAG);
        if (mSensor == null) {
            Log.i(TAG, "Pickup sensor is not detected");
        } else {
            mExecutorService = Executors.newSingleThreadExecutor();
        }
    }

    private Future<?> submit(Runnable runnable) {
        return mExecutorService.submit(runnable);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        boolean isAOD = DozeUtils.isAlwaysOnEnabled(mContext);
        boolean isPickUp = DozeUtils.isPickUpEnabled(mContext);
        boolean isSmartWake = DozeUtils.isSmartWakeEnabled(mContext);

        if (DEBUG) Log.d(TAG, "Got sensor event: " + event.values[0]);

        long delta = SystemClock.elapsedRealtime() - mEntryTimestamp;

        if (isPickUp && isSmartWake && !isAOD) {
            if (!doneDelay) {
                if (delta < MIN_PULSE_INTERVAL_MS) {
                    return;
                }
                mEntryTimestamp = SystemClock.elapsedRealtime();
                doneDelay = true;
            }
            if (event.values[0] == 2) {
                DozeUtils.launchDozePulse(mContext);
            }
            mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_TILT_DETECTOR);
            if (event.values[0] == 0) {
                mSensor = DozeUtils.getSensor(mSensorManager, "qti.sensor.amd");
                mWakeLock.acquire(WAKELOCK_TIMEOUT_MS);
                mPowerManager.wakeUp(SystemClock.uptimeMillis(),
                        PowerManager.WAKE_REASON_GESTURE, TAG);
            } else {
                mSensorManager.registerListener(this, mSensor,
                        SensorManager.SENSOR_DELAY_NORMAL);
                mSensor = DozeUtils.getSensor(mSensorManager, "qti.sensor.amd");
            }
        } else {
            if (delta < MIN_PULSE_INTERVAL_MS) {
                return;
            }
            mEntryTimestamp = SystemClock.elapsedRealtime();
            if (event.values[0] == 2) {
                DozeUtils.launchDozePulse(mContext);
                if (DEBUG) Log.d(TAG, "Motion detected");
            }
            else if (event.values[0] == 1)
            {
                if (DEBUG) Log.d(TAG, "Waiting for motion detection");
            }
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        /* Empty */
    }

    protected void enable() {
        doneDelay = false;
        if (DEBUG) Log.d(TAG, "Enabling");
        submit(() -> {
            mEntryTimestamp = SystemClock.elapsedRealtime();
            mSensorManager.registerListener(this, mSensor,
                    SensorManager.SENSOR_DELAY_NORMAL);
        });
    }

    protected void disable() {
        if (DEBUG) Log.d(TAG, "Disabling");
        submit(() -> {
            mSensorManager.unregisterListener(this, mSensor);
        });
    }
}
