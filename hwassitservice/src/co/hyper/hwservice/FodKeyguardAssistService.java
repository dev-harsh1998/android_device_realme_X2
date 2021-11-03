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
import android.util.Log;

public class FodKeyguardAssistService extends Service {
    private static final String TAG = "FodKeyguardAssistService";
    private static final boolean DEBUG = true;
    private static final String STATUS = "/sys/kernel/oppo_display/dimlayer_hbm";

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if(intent.getAction().equals(Intent.ACTION_USER_PRESENT)) {
                if (DEBUG) Log.d(TAG, "Device Unlocked");

                if (FileHelper.getFileValue(STATUS, "1") != "0")
                    FileHelper.writeValue(STATUS, "0");
            }
        }
    };

    @Override
    public void onCreate() {
       if (DEBUG) Log.d(TAG, "Creating callback for DimLayerDisablerService");
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_USER_PRESENT);
        registerReceiver(mIntentReceiver, filter);
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