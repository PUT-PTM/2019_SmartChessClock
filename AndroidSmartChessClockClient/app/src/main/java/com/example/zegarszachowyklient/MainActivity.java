package com.example.zegarszachowyklient;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.os.CountDownTimer;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

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
    public boolean bConnection = false;

    public BluetoothAdapter bAdapter;

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

    TextView p1Time;
    TextView p2Time;
    TextView p1Moves;
    TextView p2Moves;
    TextView p1LastTurn;
    TextView p2LastTurn;
    TextView TimePreset;
    Button startStop;
    Button Turn;
    CountDownTimer p1;
    CountDownTimer p2;
    ImageView Bluetooth_button;

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
        //Reset = (Button) findViewById(R.id.Reset);
        p1Time.setText(displayTime(p1time));
        p2Time.setText(displayTime(p2time));
        p1LastTurn.setText("(-" + displayTime(0) + ")");
        p2LastTurn.setText("(-" + displayTime(0) + ")");
        TimePreset.setText(displayTime(currentTP.getOverallTime()) + " + " + displayTime(currentTP.getIncrementation()));

        bAdapter = BluetoothAdapter.getDefaultAdapter();
        Thread btUpdater = new Thread(){
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
        btUpdater.start();

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
}
