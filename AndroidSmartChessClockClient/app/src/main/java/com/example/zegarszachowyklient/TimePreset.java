package com.example.zegarszachowyklient;

public class TimePreset {

    private long overallTime; //in ms
    private long incrementation; //in ms

    public TimePreset(long overallTime, long incrementation)
    {
        if(incrementation >= 0)
        {
            this.overallTime = overallTime;
            this.incrementation = incrementation;
        }
        else
        {
            this.overallTime = overallTime;
            this.incrementation = 0;
        }
    }

    public long getOverallTime()
    {
        return  overallTime;
    }

    public long getIncrementation()
    {
        return incrementation;
    }
}
