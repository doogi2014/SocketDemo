package cn.doogi.socketclientandroid;

import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class MySocket {
    private static final String TAG = "MySocket";
    private Socket socket;
    private InputStream inputStream;
    private OutputStream outputStream;
    private ReceiveThread receiveThread = null;
    private SendThread sendThread = null;

    private Queue<byte[]> sendQueue = null;

    public void start( final String host, final int port) {


        new Thread() {
            @Override
            public void run() {
                try {
                    socket = new Socket(host, port);
                    inputStream = socket.getInputStream();
                    outputStream = socket.getOutputStream();
                    outputStream.write("start\r\n\r\n".getBytes());
                    outputStream.flush();

                    receiveThread = new ReceiveThread();
                    receiveThread.start();
                    sendQueue = new ConcurrentLinkedQueue<>();
                    sendThread = new SendThread();
                    sendThread.start();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }.start();
    }

    public void stop(){
        if(sendThread != null){
            sendThread.stopThread();
        }

        if(receiveThread != null){
            receiveThread.stopThread();
        }

        if(outputStream !=null){
            try {
                outputStream.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        if(inputStream !=null){
            try {
                inputStream.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        if(socket !=null){
            try {
                socket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

    public void sendEvent(byte[] buf) {
        sendQueue.add(buf);
    }


    private void send(byte[] buf){
        try {
            outputStream.write(buf);
            outputStream.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    private class SendThread extends Thread {

        private boolean isRunning;
        public void stopThread() {
            isRunning = false;
        }

        @Override
        public void run() {
            isRunning = true;
            while (isRunning) {
                if(sendQueue.size()>0){
                    send(sendQueue.poll());
                }else {
                    try {
                        sleep(1);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    private int count = 0;
    private class ReceiveThread extends Thread {

        private boolean isRunning;
        public void stopThread() {
            isRunning = false;
        }

        @Override
        public void run() {

            byte[] bufRec = new byte[8*1024];
            int bufLen = 0;
            int idxAll = 0;

            isRunning = true;
            while (isRunning) {
                count++;
                try {
                    // 从socket读取数据，放入到缓冲区
                    int len = inputStream.read(bufRec);
                    Log.d(TAG, "socket read: " + len);
                    if(len < 0) {
                        Log.d(TAG, "server closed, read thread exit.");
                        break;
                    }
                    if (len > 0) {
                        //解析数据
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
