package com.example.zegarszachowyklient;

import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.Context;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.UUID;

public class BluetoothConnection {
    private static final String TAG = "BlueetoothConnection";

    private static final String appName = "Smart Chess Clock";

    private static final UUID MY_UUID_INSECURE = UUID.fromString("d1fc1fc0-2346-4de5-b4d8-a52fd2a5900a");

    private final BluetoothAdapter bAdapter;
    Context mContext;

    private AcceptThread bInsecureAccpetThread;
    private ConnectThread bConnectThread;
    private BluetoothDevice bDevice;
    private UUID deviceUUID;
    public ProgressDialog bProgressDialog;
    public boolean connected = false;
    public byte[] lastMSG;

    private ConnectedThread bConnectedThread;

    public BluetoothConnection(Context context)
    {
        mContext = context;
        bAdapter = BluetoothAdapter.getDefaultAdapter();
        start();
    }

    private class AcceptThread extends Thread{
        private final BluetoothServerSocket serverSocket;

        public AcceptThread()
        {
            BluetoothServerSocket tmp = null;

            try {
                tmp = bAdapter.listenUsingInsecureRfcommWithServiceRecord(appName, MY_UUID_INSECURE);
            } catch (IOException e) {
                e.printStackTrace();
            }

            serverSocket = tmp;
        }

        public void run()
        {
            BluetoothSocket socket = null;

            try {
                socket = serverSocket.accept();
            } catch (IOException e) {
                e.printStackTrace();
            }

            if (socket != null)
            {
                connected(socket,bDevice);
            }
        }

        public void cancel()
        {
            try {
                serverSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private class ConnectThread extends Thread
    {
        private BluetoothSocket bSocket;

        public ConnectThread(BluetoothDevice device, UUID uuid)
        {
            bDevice = device;
            deviceUUID = uuid;
        }

        public void run()
        {
            BluetoothSocket tmp = null;

            try {
                tmp = bDevice.createRfcommSocketToServiceRecord(deviceUUID);


            } catch (IOException e) {
                e.printStackTrace();
            }
            bSocket = tmp;
            bAdapter.cancelDiscovery();

            try {
                bSocket.connect();
                //tmp.connect();
            } catch (IOException e) {
                try {
                    //bSocket.close();
                    bSocket = (BluetoothSocket) bDevice.getClass().getMethod("createRfcommSocket", new Class[]{int.class}).invoke(bDevice,1);
                    bSocket.connect();

                } catch (IllegalAccessException e1) {
                    e1.printStackTrace();
                } catch (InvocationTargetException e1) {
                    e1.printStackTrace();
                } catch (NoSuchMethodException e1) {
                    e1.printStackTrace();
                } catch (IOException e1) {
                    e1.printStackTrace();
                }
            }
            connected(bSocket, bDevice);
        }

        public void cancel()
        {
            try {
                bSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public synchronized void start()
    {
        if(bConnectThread != null)
        {
            bConnectThread.cancel();
            bConnectThread = null;
        }
        if(bInsecureAccpetThread == null)
        {
            bInsecureAccpetThread = new AcceptThread();
            bInsecureAccpetThread.start();
        }
    }

    public void startClient(BluetoothDevice device, UUID uuid)
    {
        bProgressDialog = ProgressDialog.show(mContext,"Conntecting Bluetooth", "Please wait...", true);

        bConnectThread = new ConnectThread(device, uuid);
        bConnectThread.start();
    }

    public class ConnectedThread extends Thread
    {
        private final BluetoothSocket bSocket;
        private final InputStream bInStream;
        private final OutputStream bOutStream;

        public ConnectedThread(BluetoothSocket socket)
        {
            bSocket = socket;
            InputStream tempIn = null;
            OutputStream tempOut = null;

            try {
                bProgressDialog.dismiss();
            } catch (NullPointerException e)
            {
                e.printStackTrace();
            }

            try {
                tempIn = bSocket.getInputStream();
                tempOut = bSocket.getOutputStream();
            } catch (IOException e) {
                e.printStackTrace();
            }

            bInStream = tempIn;
            bOutStream = tempOut;
        }

        public void run()
        {
            byte[] buffer = new byte[1024];
            int bytesRead;

            while(true)
            {
                try {
                    bytesRead = bInStream.read(buffer);
                    if(bytesRead>0)
                        lastMSG = buffer;
                } catch (IOException e) {
                    e.printStackTrace();
                    break;
                }
            }
        }

        public void write(byte[] bytes)
        {
            try {
                bOutStream.write(bytes);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        public void cancel()
        {
            try {
                bSocket.close();
                connected = false;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private void connected(BluetoothSocket socket,BluetoothDevice device)
    {
        bConnectedThread = new ConnectedThread(socket);
        bConnectedThread.start();
        connected = true;
    }

    public void write(byte[] message)
    {
        bConnectedThread.write(message);
    }

    public void  cancel()
    {
        if (bInsecureAccpetThread != null)
            bInsecureAccpetThread.cancel();
        if (bConnectThread != null)
            bConnectThread.cancel();
        if (bConnectedThread != null)
            bConnectedThread.cancel();
    }
}
