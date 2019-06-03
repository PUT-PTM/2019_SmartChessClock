package com.example.zegarszachowyklient;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.drawable.ColorDrawable;
import android.os.Build;
import android.os.CountDownTimer;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.PopupWindow;
import android.widget.TextView;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.UUID;

public class MainActivity extends AppCompatActivity{
    //Clock variables
    public static final String EXTRA_MESSAGE = "com.example.zegarszachowyklient.MESSAGE";
    public static TimePreset currentTP = new TimePreset(300000, 3000);
    private boolean running = false;
    private boolean turn = false;
    private int p1moves = 0;
    private int p2moves = 0;
    private long p1time = currentTP.getOverallTime();
    private long p2time = currentTP.getOverallTime();
    private long p1lastturn = 0;
    private long p2lastturn = 0;
    private boolean bConnection = false;
    private boolean bInterfaceVisible = true;
    private boolean bConnectButtonVisible = false;
    //Bluetooth variables
    public BluetoothConnection bBluetoothConnection = new BluetoothConnection(MainActivity.this);
    public ArrayList<BluetoothDevice> bDevices = new ArrayList<>();
    public DeviceListAdapter bDeviceListAdapter;
    public ListView lvNewDevices;
    private static final UUID MY_UUID_INSECURE = UUID.fromString("d1fc1fc0-2346-4de5-b4d8-a52fd2a5900a");

    public BluetoothDevice bDevice;
    public BluetoothAdapter bAdapter;

    public void showNoTime(boolean player)
    {
        LayoutInflater inflater = (LayoutInflater) MainActivity.this.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View layout = inflater.inflate(R.layout.notime,null);
        if(player)
            ((TextView)layout.findViewById(R.id.noTimeText)).setText("Time of the 1st player run out!");
        else
            ((TextView)layout.findViewById(R.id.noTimeText)).setText("Time of the 2nd player run out!");
        float density=MainActivity.this.getResources().getDisplayMetrics().density;
        final PopupWindow pw = new PopupWindow(layout, (int)density*240, (int)density*150, true);
        pw.setBackgroundDrawable(new ColorDrawable(android.graphics.Color.TRANSPARENT));
        pw.setTouchInterceptor(new View.OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                if(event.getAction() == MotionEvent.ACTION_OUTSIDE) {
                    pw.dismiss();
                    return true;
                }
                return false;
            }
        });
        pw.setOutsideTouchable(true);
        pw.showAtLocation(layout, Gravity.CENTER, 0, 0);
    }


    public static byte[] intToByteArray(int a)
    {
        byte[] ret = new byte[4];
        ret[3] = (byte) (a & 0xFF);
        ret[2] = (byte) ((a >> 8) & 0xFF);
        ret[1] = (byte) ((a >> 16) & 0xFF);
        ret[0] = (byte) ((a >> 24) & 0xFF);
        return ret;
    }

    //Messages to send over Bluetooth
    private class Message
    {
        private final byte code;
        private final int time1;
        private final int time2;

        public Message (byte[] encrypted)
        {
            this.code = encrypted[0];
            byte[] time1Encrypted = {encrypted[1], encrypted[2], encrypted[3],encrypted[4]};
            ByteBuffer converter = ByteBuffer.wrap(time1Encrypted);
            time1 = converter.getInt();
            byte[] time2Encrypted = {encrypted[5], encrypted[6], encrypted[7],encrypted[8]};
            converter = ByteBuffer.wrap(time2Encrypted);
            time2 = converter.getInt();
        }

        public Message (int code, int time1, int time2)
        {
            this.code = (byte)code;
            this.time1 = time1;
            this.time2 = time2;
        }

        public byte[] encrypt()
        {
            byte[] time1Encrypted = intToByteArray(time1);
            byte[] time2Encrypted = intToByteArray(time2);
            byte[] toReturn = new byte[9];
            toReturn[0] = code;
            for(int i = 1; i < 5;i++)
                toReturn[i] = time1Encrypted[i-1];
            for(int i = 5; i < 9;i++)
                toReturn[i] = time1Encrypted[i-5];
            return toReturn;
        }
    }

    //Transforming time into readable format
    private String displayTime(long time)
    {
        String buffer = "";
        if (time >= 3600000)
        {

        }
        else
        {
            if(time >= 60000)
            {
                int minutes = (int)time/60000;
                int seconds = (int) (time-(60000*minutes))/1000;
                if(seconds < 10)
                    buffer = minutes + ":0" + seconds;
                else
                    buffer = minutes + ":" + seconds;
                return buffer;
            }
            else
            {
                int seconds = (int)time/1000;
                int miliseconds = (int)(time - seconds*1000);
                buffer = seconds + ".";
                if(miliseconds < 100)
                    buffer+="0";
                if(miliseconds < 10)
                    buffer+="0";
                buffer += miliseconds;
                return  buffer;
            }
        }
        return buffer;
    }

    //Layout Elements
    TextView p1Time;
    TextView p2Time;
    TextView p1Moves;
    TextView p2Moves;
    TextView p1LastTurn;
    TextView p2LastTurn;
    TextView TimePreset;
    Button startStop;
    Button Turn;
    Button bStartConnection;
    Button bDiscover;
    CountDownTimer p1;
    CountDownTimer p2;
    ImageView Bluetooth_button;

    private final BroadcastReceiver bBroadCastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if(action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED))
            {
                BluetoothDevice btDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if(btDevice.getBondState() == BluetoothDevice.BOND_BONDED)
                {
                    Log.d("DEBUG", "BroadCastReceiver: BOND_BONDED");
                    bDevice = btDevice;
                }
                if(btDevice.getBondState() == BluetoothDevice.BOND_BONDING)
                {
                    Log.d("DEBUG", "BroadCastReceiver: BOND_BONDING");
                }
                if(btDevice.getBondState() == BluetoothDevice.BOND_NONE)
                {
                    Log.d("DEBUG", "BroadCastReceiver: BOND_NONE");
                }
            }
        }
    };

    private final BroadcastReceiver bBroadCastReceiver2 = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if(action.equals(BluetoothDevice.ACTION_FOUND))
            {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                bDevices.add(device);
                bDeviceListAdapter = new DeviceListAdapter(MainActivity.this, R.layout.device_adapter_view, bDevices);
                lvNewDevices.setAdapter(bDeviceListAdapter);
                lvNewDevices.setOnItemClickListener(new ListView.OnItemClickListener(){
                    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l)
                {
                    bAdapter.cancelDiscovery();
                    String deviceName = bDevices.get(i).getName();
                    String deviceAddress = bDevices.get(i).getAddress();

                    bConnectButtonVisible = true;
                    bStartConnection.setText("Connect\nwith\n" + deviceName);
                    updateConnectButtonVisibility();

                    if(Build.VERSION.SDK_INT > Build.VERSION_CODES.JELLY_BEAN_MR2)
                    {

                        bDevices.get(i).createBond();

                        bDevice = bDevices.get(i);
                        bBluetoothConnection = new BluetoothConnection(MainActivity.this);
                    }
                }});
            }
        }
    };

    //UI update functions
    void updateConnectButtonVisibility()
    {
        if (bConnectButtonVisible)
            bStartConnection.setVisibility(View.VISIBLE);
        else
            bStartConnection.setVisibility(View.INVISIBLE);
    }

    void updateBluetoothButton()
    {
        if(bAdapter == null)
        {
            Bluetooth_button.setImageDrawable(getResources().getDrawable(R.drawable.bluetooth_notavailable));
        }
        else if (bAdapter.isEnabled())
        {
            if(bConnection)
                Bluetooth_button.setImageDrawable(getResources().getDrawable(R.drawable.bluetooth_connected));
            else
                Bluetooth_button.setImageDrawable(getResources().getDrawable(R.drawable.bluetooth_enabled));
        }
        else
        {
            Bluetooth_button.setImageDrawable(getResources().getDrawable(R.drawable.bluetooth_disabled));
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //connecting objects with their visual counterparts
        lvNewDevices = findViewById(R.id.lvNewDevices);
        p1Time = (TextView) findViewById(R.id.Time_P1);
        p2Time = (TextView) findViewById(R.id.Time_P2);
        p1Moves = (TextView) findViewById(R.id.Moves_P1);
        p2Moves = (TextView) findViewById(R.id.Moves_P2);
        p1LastTurn = (TextView) findViewById(R.id.p1LastTurn);
        p2LastTurn = (TextView) findViewById(R.id.p2LastTurn);
        TimePreset = (TextView) findViewById(R.id.PresetDisplay);
        startStop = (Button) findViewById(R.id.Start_Stop);
        Turn = (Button) findViewById(R.id.Turn);
        Bluetooth_button = findViewById(R.id.Bluetooth_button);
        bStartConnection = findViewById(R.id.bStartConnection);
        bDiscover = findViewById(R.id.bDiscover);
        lvNewDevices = findViewById(R.id.lvNewDevices);

        //displaying initial values on screen
        p1Time.setText(displayTime(p1time));
        p2Time.setText(displayTime(p2time));
        p1LastTurn.setText("(-" + displayTime(0) + ")");
        p2LastTurn.setText("(-" + displayTime(0) + ")");
        TimePreset.setText(displayTime(currentTP.getOverallTime()) + " + " + displayTime(currentTP.getIncrementation()));
        updateConnectButtonVisibility();

        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        registerReceiver(bBroadCastReceiver, filter);

        bAdapter = BluetoothAdapter.getDefaultAdapter();

        //Additional Threads

        //Updating bluetooth Icon
        final Thread btUpdater = new Thread(){
            public void run()
            {
                while(true)
                {
                    try {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                updateBluetoothButton();
                            }
                        });
                        Thread.sleep(100);
                    }
                    catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        };

        //Handling incoming messages
        Thread MessageInterpreter = new Thread()
        {
            @Override
            public void run() {
                while (true) {
                    try{
                    if (bBluetoothConnection.connected) {
                        Message receivedMSG = new Message(bBluetoothConnection.lastMSG);
                        switch (receivedMSG.code) {
                            case 83: {
                                p1time = (long) receivedMSG.time1;
                                p2time = (long) receivedMSG.time2;
                                if (running) {
                                    running = false;
                                    p1.cancel();
                                    p2.cancel();
                                } else {
                                    running = true;
                                    if (!turn) {
                                        p1 = new CountDownTimer(p1time, 10) {
                                            @Override
                                            public void onTick(long millisUntilFinished) {
                                                p1time = millisUntilFinished;
                                                p1Time.setText(displayTime(millisUntilFinished));
                                                p1lastturn += 10;
                                                p1LastTurn.setText("(-" + displayTime(p1lastturn) + ")");
                                            }

                                            @Override
                                            public void onFinish() {
                                                running = false;
                                                p1time = 0;
                                                p1Time.setText(displayTime(0));
                                                p1lastturn += 10;
                                                p1LastTurn.setText("(-" + displayTime(p1lastturn) + ")");
                                                showNoTime(true);
                                            }

                                        };
                                        p2 = new CountDownTimer(10, 10) {
                                            @Override
                                            public void onTick(long millisUntilFinished) {
                                            }

                                            @Override
                                            public void onFinish() {
                                            }
                                        };
                                        p2.start();
                                        p2.cancel();
                                        p1.start();
                                    } else {
                                        p2 = new CountDownTimer(p2time, 10) {
                                            @Override
                                            public void onTick(long millisUntilFinished) {
                                                p2time = millisUntilFinished;
                                                p2Time.setText(displayTime(millisUntilFinished));
                                                p2lastturn += 10;
                                                p2LastTurn.setText("(-" + displayTime(p2lastturn) + ")");
                                            }

                                            @Override
                                            public void onFinish() {
                                                running = false;
                                                p2time = 0;
                                                p2Time.setText(displayTime(0));
                                                p2lastturn += 10;
                                                p2LastTurn.setText("(-" + displayTime(p2lastturn) + ")");
                                                showNoTime(false);
                                            }
                                        };
                                        p2.start();
                                    }
                                }
                                break;
                            }
                            case 84: {
                                p1time = (long) receivedMSG.time1;
                                p2time = (long) receivedMSG.time2;
                                if (running) {
                                    if (turn) {
                                        turn = false;
                                        p2.cancel();
                                        p2moves++;
                                        p2Moves.setText(Long.toString(p2moves));
                                        p2time += currentTP.getIncrementation();
                                        p2Time.setText(displayTime(p2time));
                                        p1lastturn = 0;
                                        p1 = new CountDownTimer(p1time, 10) {
                                            @Override
                                            public void onTick(long millisUntilFinished) {
                                                p1time = millisUntilFinished;
                                                p1Time.setText(displayTime(millisUntilFinished));
                                                p1lastturn += 10;
                                                p1LastTurn.setText("(-" + displayTime(p1lastturn) + ")");
                                            }

                                            @Override
                                            public void onFinish() {
                                                running = false;
                                                p1time = 0;
                                                p1Time.setText(displayTime(0));
                                                p1lastturn += 10;
                                                p1LastTurn.setText("(-" + displayTime(p1lastturn) + ")");
                                                showNoTime(true);
                                            }
                                        };
                                        p1.start();
                                    } else {
                                        turn = true;
                                        p1.cancel();
                                        p1moves++;
                                        p1Moves.setText(Long.toString(p1moves));
                                        p1time += currentTP.getIncrementation();
                                        p1Time.setText(displayTime(p1time));
                                        p2lastturn = 0;
                                        p2 = new CountDownTimer(p2time, 10) {
                                            @Override
                                            public void onTick(long millisUntilFinished) {
                                                p2time = millisUntilFinished;
                                                p2Time.setText(displayTime(millisUntilFinished));
                                                p2lastturn += 10;
                                                p2LastTurn.setText("(-" + displayTime(p2lastturn) + ")");
                                            }

                                            @Override
                                            public void onFinish() {
                                                running = false;
                                                p2time = 0;
                                                p2Time.setText(displayTime(0));
                                                p2lastturn += 10;
                                                p2LastTurn.setText("(-" + displayTime(p2lastturn) + ")");
                                                showNoTime(false);
                                            }
                                        };
                                        p2.start();
                                    }
                                } else {

                                }
                                break;
                            }
                            default: {
                                break;
                            }
                        }
                    }
                    try {
                        Thread.sleep(10);
                    } catch (InterruptedException e) {
                    e.printStackTrace();
                    }
                }
                catch(NullPointerException e)
                {

                }
                }

            }
        };



        Bluetooth_button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(bInterfaceVisible)
                {
                    bInterfaceVisible = false;
                    lvNewDevices.setVisibility(View.INVISIBLE);
                    bStartConnection.setVisibility(View.INVISIBLE);
                    bDiscover.setVisibility(View.INVISIBLE);
                }
                else
                {
                    bInterfaceVisible = true;
                    lvNewDevices.setVisibility(View.VISIBLE);
                    bStartConnection.setVisibility(View.VISIBLE);
                    bDiscover.setVisibility(View.VISIBLE);
                }
            }
        });

        btUpdater.start();
        MessageInterpreter.start();
        bStartConnection.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startConnection();
            }
        });
        startStop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(running)
                {
                    running =  false;
                    p1.cancel();
                    p2.cancel();
                    return;
                }
                else
                {
                    running =  true;
                    if(!turn)
                    {
                        p1 = new CountDownTimer(p1time, 10) {
                            @Override
                            public void onTick(long millisUntilFinished) {
                                p1time = millisUntilFinished;
                                p1Time.setText(displayTime(millisUntilFinished));
                                p1lastturn += 10;
                                p1LastTurn.setText("(-" + displayTime(p1lastturn) + ")");
                            }

                            @Override
                            public void onFinish() {
                                running = false;
                                p1time = 0;
                                p1Time.setText(displayTime(0));
                                p1lastturn += 10;
                                p1LastTurn.setText("(-" + displayTime(p1lastturn) + ")");
                                showNoTime(true);
                            }

                        };
                        p2 = new CountDownTimer(10, 10) {
                            @Override
                            public void onTick(long millisUntilFinished) {
                            }

                            @Override
                            public void onFinish() {
                            }
                        };
                        p2.start();
                        p2.cancel();
                        p1.start();
                    }
                    else
                    {
                        p2 = new CountDownTimer(p2time, 10) {
                            @Override
                            public void onTick(long millisUntilFinished) {
                                p2time = millisUntilFinished;
                                p2Time.setText(displayTime(millisUntilFinished));
                                p2lastturn += 10;
                                p2LastTurn.setText("(-" + displayTime(p2lastturn) + ")");
                            }

                            @Override
                            public void onFinish() {
                                running = false;
                                p2time = 0;
                                p2Time.setText(displayTime(0));
                                p2lastturn += 10;
                                p2LastTurn.setText("(-" + displayTime(p2lastturn) + ")");
                                showNoTime(false);
                            }
                        };
                        p2.start();
                    }
                }
            }
        });
        Turn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(running)
                {
                    if(turn)
                    {
                        turn = false;
                        p2.cancel();
                        p2moves ++;
                        p2Moves.setText(Long.toString(p2moves));
                        p2time += currentTP.getIncrementation();
                        p2Time.setText(displayTime(p2time));
                        p1lastturn = 0;
                        p1 = new CountDownTimer(p1time, 10) {
                            @Override
                            public void onTick(long millisUntilFinished) {
                                p1time = millisUntilFinished;
                                p1Time.setText(displayTime(millisUntilFinished));
                                p1lastturn += 10;
                                p1LastTurn.setText("(-" + displayTime(p1lastturn) + ")");
                            }

                            @Override
                            public void onFinish() {
                                running = false;
                                p1time = 0;
                                p1Time.setText(displayTime(0));
                                p1lastturn += 10;
                                p1LastTurn.setText("(-" + displayTime(p1lastturn) + ")");
                                showNoTime(true);
                            }
                        };
                        p1.start();
                        return;
                    }
                    else
                    {
                        turn = true;
                        p1.cancel();
                        p1moves ++;
                        p1Moves.setText(Long.toString(p1moves));
                        p1time += currentTP.getIncrementation();
                        p1Time.setText(displayTime(p1time));
                        p2lastturn = 0;
                        p2 = new CountDownTimer(p2time, 10) {
                            @Override
                            public void onTick(long millisUntilFinished) {
                                p2time = millisUntilFinished;
                                p2Time.setText(displayTime(millisUntilFinished));
                                p2lastturn += 10;
                                p2LastTurn.setText("(-" + displayTime(p2lastturn) + ")");
                            }

                            @Override
                            public void onFinish() {
                                running = false;
                                p2time = 0;
                                p2Time.setText(displayTime(0));
                                p2lastturn += 10;
                                p2LastTurn.setText("(-" + displayTime(p2lastturn) + ")");
                                showNoTime(false);
                            }
                        };
                        p2.start();
                        return;
                    }
                }
                else
                {
                    return;
                }
            }
        });
    }
    public void sendMessage(View view) {
        if(!running)
        {
            Intent intent = new Intent(this, PresetCreatorActivity.class);
            startActivity(intent);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        TimePreset.setText(displayTime(currentTP.getOverallTime()) + " + " + displayTime(currentTP.getIncrementation()));
        if (bAdapter.isEnabled()) {
            Message presetToSend = new Message(80, (int) currentTP.getOverallTime(), (int) currentTP.getIncrementation());
            byte[] encrypted = presetToSend.encrypt();
            if(bBluetoothConnection !=null)
                if(bBluetoothConnection.connected)
                    bBluetoothConnection.write(encrypted);
        }
        p1moves = 0;
        p1Moves.setText(Long.toString(p1moves));
        p2moves = 0;
        p2Moves.setText(Long.toString(p2moves));
        p1time = currentTP.getOverallTime();
        p2time = currentTP.getOverallTime();
        p1Time.setText(displayTime(p1time));
        p2Time.setText(displayTime(p2time));
        p1lastturn = 0;
        p2lastturn = 0;
        p1LastTurn.setText("(-" + displayTime(0) + ")");
        p2LastTurn.setText("(-" + displayTime(0) + ")");
        turn = false;
    }

    public void startConnection()
    {
        startBTConnection(bDevice, MY_UUID_INSECURE);
    }

    public void startBTConnection(BluetoothDevice device, UUID uuid)
    {
        bBluetoothConnection.startClient(device,uuid);
    }

    public void bDiscover(View view)
    {
        bConnectButtonVisible = false;
        updateConnectButtonVisibility();
        bDevices = new ArrayList<>();
        if(bAdapter.isDiscovering())
        {
            bAdapter.cancelDiscovery();
            checkBTPermissions();

            bAdapter.startDiscovery();
            IntentFilter discoverDevicesIntent = new IntentFilter(BluetoothDevice.ACTION_FOUND);
            registerReceiver(bBroadCastReceiver2, discoverDevicesIntent);
        }
        if(!bAdapter.isDiscovering())
        {
            checkBTPermissions();
            bAdapter.startDiscovery();
            IntentFilter discoverDevicesIntent = new IntentFilter(BluetoothDevice.ACTION_FOUND);
            registerReceiver(bBroadCastReceiver2, discoverDevicesIntent);
        }
    }

    private void checkBTPermissions() {
        if(Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP){
            int permissionCheck = 0;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                permissionCheck = this.checkSelfPermission("Manifest.permission.ACCESS_FINE_LOCATION");
            }
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                permissionCheck += this.checkSelfPermission("Manifest.permission.ACCESS_COARSE_LOCATION");
            }
            if (permissionCheck != 0) {

                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                    this.requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION}, 1001); //Any number
                }
            }
        }else{
            Log.d("DEBUG", "checkBTPermissions: No need to check permissions. SDK version < LOLLIPOP.");
        }
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        unregisterReceiver(bBroadCastReceiver);
        unregisterReceiver(bBroadCastReceiver2);
        bBluetoothConnection.bProgressDialog.dismiss();
        bBluetoothConnection.cancel();
    }
}
