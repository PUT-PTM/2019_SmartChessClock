package com.example.zegarszachowyklient;

import android.os.CountDownTimer;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private boolean running = false;
    private boolean turn = false;
    private int p1moves = 0;
    private int p2moves = 0;
    private long p1time = 300000;
    private long p2time = 300000;

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
                buffer = minutes + ":" + seconds;
                return buffer;
            }
            else
            {
                int seconds = (int)time/1000;
                int miliseconds = (int)(time - seconds*1000);
                buffer = seconds + "." + miliseconds;
            }
        }
        return buffer;
    }

    TextView p1Time;
    TextView p2Time;
    TextView p1Moves;
    TextView p2Moves;
    Button startStop;
    Button Turn;
    CountDownTimer p1;
    CountDownTimer p2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        p1Time = (TextView) findViewById(R.id.Time_P1);
        p2Time = (TextView) findViewById(R.id.Time_P2);
        p1Moves = (TextView) findViewById(R.id.Moves_P1);
        p2Moves = (TextView) findViewById(R.id.Moves_P2);
        startStop = (Button) findViewById(R.id.Start_Stop);
        Turn = (Button) findViewById(R.id.Turn);
        p1Time.setText(displayTime(p1time));
        p2Time.setText(displayTime(p2time));
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
                            }

                            @Override
                            public void onFinish() {
                                running = false;
                                p1time = 0;
                                p1Time.setText(displayTime(0));
                            }
                        };
                    }
                    else
                    {
                        p2 = new CountDownTimer(p2time, 10) {
                            @Override
                            public void onTick(long millisUntilFinished) {
                                p2time = millisUntilFinished;
                                p2Time.setText(displayTime(millisUntilFinished));
                            }

                            @Override
                            public void onFinish() {
                                running = false;
                                p2time = 0;
                                p2Time.setText(displayTime(0));
                            }
                        };
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
                        p1 = new CountDownTimer(p1time, 10) {
                            @Override
                            public void onTick(long millisUntilFinished) {
                                p1time = millisUntilFinished;
                                p1Time.setText(displayTime(millisUntilFinished));
                            }

                            @Override
                            public void onFinish() {
                                running = false;
                                p1time = 0;
                                p1Time.setText(displayTime(0));
                            }
                        };
                        return;
                    }
                    else
                    {
                        turn = true;
                        p1.cancel();
                        p1moves ++;
                        p1Moves.setText(Long.toString(p1moves));
                        p2 = new CountDownTimer(p2time, 10) {
                            @Override
                            public void onTick(long millisUntilFinished) {
                                p2time = millisUntilFinished;
                                p2Time.setText(displayTime(millisUntilFinished));
                            }

                            @Override
                            public void onFinish() {
                                running = false;
                                p2time = 0;
                                p2Time.setText(displayTime(0));
                            }
                        };
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
}
