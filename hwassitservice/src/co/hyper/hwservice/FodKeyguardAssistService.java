/*
 * Copyright (c) 2021 Mohit Sethi <mohitsethi.32@gmail.com>
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
 * 
 * Purpose: DimLayerDisablerService class, starts the required listeners.
 * 
 */

package co.hyper.hwservice;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

// Import and use the oppo biometric hal
import vendor.oppo.hardware.biometrics.fingerprint.V2_1.IBiometricsFingerprint;

public class FodKeyguardAssistService extends Service {
    private static final String TAG = "FodKeyguardAssistService";
    private static final boolean DEBUG = false;
    private static final String STATUS = "/sys/kernel/oppo_display/dimlayer_hbm";
    private static IBiometricsFingerprint mOppoFingerprint;

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if(intent.getAction().equals(Intent.ACTION_USER_PRESENT)) {
                if (DEBUG) Log.d(TAG, "Device Unlocked");

                final boolean requireDimDead = (FileHelper.getFileValue(STATUS, "1") != "0");

                if (requireDimDead) {
                    FileHelper.writeValue(STATUS, "0");

                // Also attempt to set screen state if the service status is not null.
                if (mOppoFingerprint != null)
                    try {
                        mOppoFingerprint.setScreenState(0);
                        if (DEBUG) Log.d(TAG, "Screen state set to 0");
                    } catch (RemoteException e) {
                        if (DEBUG) Log.d(TAG, "Failed to set screen state");
                    }
                }
            }
        }
    };

    @Override
    public void onCreate() {
       if (DEBUG) Log.d(TAG, "Creating callback for DimLayerDisablerService");
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_USER_PRESENT);
        registerReceiver(mIntentReceiver, filter);
        // Attempt to get the oppo biometric hal as service to use SetScreenState.
        try {
            mOppoFingerprint = IBiometricsFingerprint.getService();
            if (mOppoFingerprint == null) {
                Log.d(TAG, "Unable to get oppo biometric hal service.");
            }
        } catch (RemoteException e) {
            if (DEBUG) Log.d(TAG, "Failed to get the oppo biometric hal.");
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        if (DEBUG) Log.d(TAG, "Destroying DimLayerDisablerService service");
        unregisterReceiver(mIntentReceiver);
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}