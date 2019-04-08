package com.example.zegarszachowyklient;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class PresetCreatorActivity extends AppCompatActivity {

    private int OMinutes = 0;
    private int OSeconds = 0;
    private int OMili = 0;
    private int IMinutes = 0;
    private int ISeconds = 0;
    private int IMili = 0;
    private int Omode = 0;
    private int Imode = 0;
    private long OconvertedTime = 0;
    private long IconvertedTime = 0;

    private String displayTime(long time)
    {
        String buffer = "";
        int minutes = (int)time/60000;
        buffer += minutes;
        int seconds = (int) (time-(60000*minutes))/1000;
        if(seconds < 10)
            buffer += ":0" + seconds;
        else
            buffer +=":" + seconds;
        int miliseconds = (int)(time-(60000*minutes) - seconds*1000);
        buffer += ".";
        if(miliseconds < 100)
            buffer+="0";
        if(miliseconds < 10)
            buffer+="0";
        buffer += miliseconds;
        return buffer;
    }

    private long convertTime(int minutes, int seconds, int miliseconds) {
        return minutes * 60000 + seconds * 1000 + miliseconds;
    }

    TextView OTime;
    TextView ITime;
    Button OMeasure;
    Button IMeasure;
    Button Osub10;
    Button Osub5;
    Button Osub1;
    Button Oadd1;
    Button Oadd5;
    Button Oadd10;
    Button Isub10;
    Button Isub5;
    Button Isub1;
    Button Iadd1;
    Button Iadd5;
    Button Iadd10;
    Button AcceptButton;
    Button Template1;
    Button Template2;
    Button Template3;
    Button Template4;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_preset_creator);
        OTime = (TextView) findViewById(R.id.OTime);
        ITime = (TextView) findViewById(R.id.ITime);
        OMeasure = (Button) findViewById(R.id.OMeasure);
        IMeasure = (Button) findViewById(R.id.IMeasure);
        Osub10 = (Button) findViewById(R.id.O_10);
        Osub5 = (Button) findViewById(R.id.O_5);
        Osub1 = (Button) findViewById(R.id.O_1);
        Oadd1 = (Button) findViewById(R.id.Oadd1);
        Oadd5 = (Button) findViewById(R.id.Oadd5);
        Oadd10 = (Button) findViewById(R.id.Oadd10);
        Isub10 = (Button) findViewById(R.id.I_10);
        Isub5 = (Button) findViewById(R.id.I_5);
        Isub1 = (Button) findViewById(R.id.I_1);
        Iadd1 = (Button) findViewById(R.id.Iadd1);
        Iadd5 = (Button) findViewById(R.id.Iadd5);
        Iadd10 = (Button) findViewById(R.id.Iadd10);
        AcceptButton = (Button) findViewById(R.id.AcceptButton);
        Template1 = (Button) findViewById(R.id.Template1);
        Template2 = (Button) findViewById(R.id.Template2);
        Template3 = (Button) findViewById(R.id.Template3);
        Template4 = (Button) findViewById(R.id.Template4);
        OTime.setText(displayTime(OconvertedTime));
        ITime.setText(displayTime(IconvertedTime));
        OTime.setText(""+OconvertedTime);
        ITime.setText(""+IconvertedTime);
        OMeasure.setText("Minutes");
        OMeasure.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Omode)
                {
                    case 0:
                    {
                        Omode++;
                        OMeasure.setText("Seconds");
                        break;
                    }
                    case 1:
                    {
                        Omode++;
                        OMeasure.setText("Milisecs");
                        Osub10.setText("-100");
                        Osub5.setText("-50");
                        Osub1.setText("-10");
                        Oadd10.setText("+100");
                        Oadd5.setText("+50");
                        Oadd1.setText("+10");
                        break;
                    }
                    case 2:
                    {
                        Omode = 0;
                        OMeasure.setText("Minutes");
                        Osub10.setText("-10");
                        Osub5.setText("-5");
                        Osub1.setText("-1");
                        Oadd10.setText("+10");
                        Oadd5.setText("+5");
                        Oadd1.setText("+1");
                        break;
                    }
                }
            }
        });
        IMeasure.setText("Minutes");
        IMeasure.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Imode)
                {
                    case 0:
                    {
                        Imode++;
                        IMeasure.setText("Seconds");
                        break;
                    }
                    case 1:
                    {
                        Imode++;
                        IMeasure.setText("Milisecs");
                        Isub10.setText("-100");
                        Isub5.setText("-50");
                        Isub1.setText("-10");
                        Iadd10.setText("+100");
                        Iadd5.setText("+50");
                        Iadd1.setText("+10");
                        break;
                    }
                    case 2:
                    {
                        Imode = 0;
                        IMeasure.setText("Minutes");
                        Isub10.setText("-10");
                        Isub5.setText("-5");
                        Isub1.setText("-1");
                        Iadd10.setText("+10");
                        Iadd5.setText("+5");
                        Iadd1.setText("+1");
                        break;
                    }
                }
            }
        });
        Osub10.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Omode)
                {
                    case 0:
                    {
                        if(OMinutes <= 10)
                            OMinutes = 0;
                        else
                            OMinutes -= 10;
                        break;
                    }
                    case 1:
                    {
                        if(OSeconds <= 10)
                            OSeconds = 0;
                        else
                            OSeconds -= 10;
                        break;
                    }
                    case 2:
                    {
                        if(OMili <= 100)
                            OMili = 0;
                        else
                            OMili -= 100;
                        break;
                    }
                }
                OconvertedTime = convertTime(OMinutes, OSeconds, OMili);
                OTime.setText(displayTime(OconvertedTime));
            }
        });
        Osub5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Omode)
                {
                    case 0:
                    {
                        if(OMinutes <= 5)
                            OMinutes = 0;
                        else
                            OMinutes -= 5;
                        break;
                    }
                    case 1:
                    {
                        if(OSeconds <= 5)
                            OSeconds = 0;
                        else
                            OSeconds -= 5;
                        break;
                    }
                    case 2:
                    {
                        if(OMili <= 50)
                            OMili = 0;
                        else
                            OMili -= 50;
                        break;
                    }
                }
                OconvertedTime = convertTime(OMinutes, OSeconds, OMili);
                OTime.setText(displayTime(OconvertedTime));
            }
        });
        Osub1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Omode)
                {
                    case 0:
                    {
                        if(OMinutes <= 1)
                            OMinutes = 0;
                        else
                            OMinutes -= 1;
                        break;
                    }
                    case 1:
                    {
                        if(OSeconds <= 1)
                            OSeconds = 0;
                        else
                            OSeconds -= 1;
                        break;
                    }
                    case 2:
                    {
                        if(OMili <= 10)
                            OMili = 0;
                        else
                            OMili -= 10;
                        break;
                    }
                }
                OconvertedTime = convertTime(OMinutes, OSeconds, OMili);
                OTime.setText(displayTime(OconvertedTime));
            }
        });
        Oadd10.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Omode)
                {
                    case 0:
                    {
                        if(OMinutes >= 90)
                            OMinutes = 99;
                        else
                            OMinutes += 10;
                        break;
                    }
                    case 1:
                    {
                        if(OSeconds >= 50)
                            OSeconds = 59;
                        else
                            OSeconds += 10;
                        break;
                    }
                    case 2:
                    {
                        if(OMili >= 900)
                            OMili = 999;
                        else
                            OMili += 100;
                        break;
                    }
                }
                OconvertedTime = convertTime(OMinutes, OSeconds, OMili);
                OTime.setText(displayTime(OconvertedTime));
            }
        });
        Oadd5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Omode)
                {
                    case 0:
                    {
                        if(OMinutes >= 95)
                            OMinutes = 99;
                        else
                            OMinutes += 5;
                        break;
                    }
                    case 1:
                    {
                        if(OSeconds >= 55)
                            OSeconds = 59;
                        else
                            OSeconds += 5;
                        break;
                    }
                    case 2:
                    {
                        if(OMili >= 950)
                            OMili = 999;
                        else
                            OMili += 50;
                        break;
                    }
                }
                OconvertedTime = convertTime(OMinutes, OSeconds, OMili);
                OTime.setText(displayTime(OconvertedTime));
            }
        });
        Oadd1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Omode)
                {
                    case 0:
                    {
                        if(OMinutes >= 99)
                            OMinutes = 99;
                        else
                            OMinutes += 1;
                        break;
                    }
                    case 1:
                    {
                        if(OSeconds >= 59)
                            OSeconds = 59;
                        else
                            OSeconds += 1;
                        break;
                    }
                    case 2:
                    {
                        if(OMili >= 999)
                            OMili = 999;
                        else
                            OMili += 10;
                        break;
                    }
                }
                OconvertedTime = convertTime(OMinutes, OSeconds, OMili);
                OTime.setText(displayTime(OconvertedTime));
            }
        });
        Isub10.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Imode)
                {
                    case 0:
                    {
                        if(IMinutes <= 10)
                            IMinutes = 0;
                        else
                            IMinutes -= 10;
                        break;
                    }
                    case 1:
                    {
                        if(ISeconds <= 10)
                            ISeconds = 0;
                        else
                            ISeconds -= 10;
                        break;
                    }
                    case 2:
                    {
                        if(IMili <= 100)
                            IMili = 0;
                        else
                            IMili -= 100;
                        break;
                    }
                }
                IconvertedTime = convertTime(IMinutes, ISeconds, IMili);
                ITime.setText(displayTime(IconvertedTime));
            }
        });
        Isub5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Imode)
                {
                    case 0:
                    {
                        if(IMinutes <= 5)
                            IMinutes = 0;
                        else
                            IMinutes -= 5;
                        break;
                    }
                    case 1:
                    {
                        if(ISeconds <= 5)
                            ISeconds = 0;
                        else
                            ISeconds -= 5;
                        break;
                    }
                    case 2:
                    {
                        if(IMili <= 50)
                            IMili = 0;
                        else
                            IMili -= 50;
                        break;
                    }
                }
                IconvertedTime = convertTime(IMinutes, ISeconds, IMili);
                ITime.setText(displayTime(IconvertedTime));
            }
        });
        Isub1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Imode)
                {
                    case 0:
                    {
                        if(IMinutes <= 1)
                            IMinutes = 0;
                        else
                            IMinutes -= 1;
                        break;
                    }
                    case 1:
                    {
                        if(ISeconds <= 1)
                            ISeconds = 0;
                        else
                            ISeconds -= 1;
                        break;
                    }
                    case 2:
                    {
                        if(IMili <= 10)
                            IMili = 0;
                        else
                            IMili -= 10;
                        break;
                    }
                }
                IconvertedTime = convertTime(IMinutes, ISeconds, IMili);
                ITime.setText(displayTime(OconvertedTime));
            }
        });
        Iadd10.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Imode)
                {
                    case 0:
                    {
                        if(IMinutes >= 90)
                            IMinutes = 99;
                        else
                            IMinutes += 10;
                        break;
                    }
                    case 1:
                    {
                        if(ISeconds >= 50)
                            ISeconds = 59;
                        else
                            ISeconds += 10;
                        break;
                    }
                    case 2:
                    {
                        if(IMili >= 900)
                            IMili = 999;
                        else
                            IMili += 100;
                        break;
                    }
                }
                IconvertedTime = convertTime(IMinutes, ISeconds, IMili);
                ITime.setText(displayTime(IconvertedTime));
            }
        });
        Iadd5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Imode)
                {
                    case 0:
                    {
                        if(IMinutes >= 95)
                            IMinutes = 99;
                        else
                            IMinutes += 5;
                        break;
                    }
                    case 1:
                    {
                        if(ISeconds >= 55)
                            ISeconds = 59;
                        else
                            ISeconds += 5;
                        break;
                    }
                    case 2:
                    {
                        if(IMili >= 950)
                            IMili = 999;
                        else
                            IMili += 50;
                        break;
                    }
                }
                IconvertedTime = convertTime(IMinutes, ISeconds, IMili);
                ITime.setText(displayTime(IconvertedTime));
            }
        });
        Iadd1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                switch (Imode)
                {
                    case 0:
                    {
                        if(IMinutes >= 99)
                            IMinutes = 99;
                        else
                            IMinutes += 1;
                        break;
                    }
                    case 1:
                    {
                        if(ISeconds >= 59)
                            ISeconds = 59;
                        else
                            ISeconds += 1;
                        break;
                    }
                    case 2:
                    {
                        if(IMili >= 999)
                            IMili = 999;
                        else
                            IMili += 10;
                        break;
                    }
                }
                IconvertedTime = convertTime(IMinutes, ISeconds, IMili);
                ITime.setText(displayTime(IconvertedTime));
            }
        });
        AcceptButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MainActivity.currentTP = new TimePreset(OconvertedTime, IconvertedTime);
                finish();
            }
        });
        Template1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                OMinutes = 5;
                OSeconds = 0;
                OMili = 0;
                IMinutes = 0;
                ISeconds = 3;
                IMili = 0;
                OconvertedTime = convertTime(OMinutes, OSeconds, OMili);
                IconvertedTime = convertTime(IMinutes, ISeconds, IMili);
                OTime.setText(displayTime(OconvertedTime));
                ITime.setText(displayTime(IconvertedTime));
            }
        });
        Template2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                OMinutes = 15;
                OSeconds = 0;
                OMili = 0;
                IMinutes = 0;
                ISeconds = 15;
                IMili = 0;
                OconvertedTime = convertTime(OMinutes, OSeconds, OMili);
                IconvertedTime = convertTime(IMinutes, ISeconds, IMili);
                OTime.setText(displayTime(OconvertedTime));
                ITime.setText(displayTime(IconvertedTime));
            }
        });
        Template3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                OMinutes = 2;
                OSeconds = 0;
                OMili = 0;
                IMinutes = 0;
                ISeconds = 1;
                IMili = 0;
                OconvertedTime = convertTime(OMinutes, OSeconds, OMili);
                IconvertedTime = convertTime(IMinutes, ISeconds, IMili);
                OTime.setText(displayTime(OconvertedTime));
                ITime.setText(displayTime(IconvertedTime));
            }
        });
        Template4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                OMinutes = 15;
                OSeconds = 0;
                OMili = 0;
                IMinutes = 0;
                ISeconds = 0;
                IMili = 0;
                OconvertedTime = convertTime(OMinutes, OSeconds, OMili);
                IconvertedTime = convertTime(IMinutes, ISeconds, IMili);
                OTime.setText(displayTime(OconvertedTime));
                ITime.setText(displayTime(IconvertedTime));
            }
        });
    }
}
