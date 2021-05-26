package cn.doogi.socketclientandroid;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {

    private  MySocket mySocket;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mySocket = new MySocket();
        mySocket.start("192.168.0.179", 9999);
        Button button = findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                byte[] buf = {0x31, 0x32};
                mySocket.sendEvent(buf);
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mySocket != null)
            mySocket.stop();
    }
}