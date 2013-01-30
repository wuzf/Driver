
package com.wzf.chardev;

import com.wzf.chardev.chardevclass;
import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.util.Log;

import android.view.View; 
import android.view.View.OnClickListener; 
import android.widget.Button; 
import android.widget.TextView; 

public class MainActivity extends Activity {
    private static final String TAG = "chardevapk";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        OnClickListener list1=new OnClickListener(){
            public void onClick(View v)
            {
                chardevclass chardevtest = new chardevclass();
                Log.i(TAG, "chardev apk read ");
                chardevtest.read();
                TextView text=(TextView)findViewById(R.id.textView1);
                CharSequence t=text.getText();
                text.setText(t+"\nRead");
            }
           };
           Button read=(Button)findViewById(R.id.button1);
           read.setOnClickListener(list1);
           OnClickListener list2=new OnClickListener(){
               public void onClick(View v)
               {
                   chardevclass chardevtest = new chardevclass();
                   Log.i(TAG, "chardev apk write ");
                   chardevtest.write(8888);
                   TextView text=(TextView)findViewById(R.id.textView1);
                   CharSequence t=text.getText();
                   text.setText(t+"\nWrite");
               }
              };
              Button write=(Button)findViewById(R.id.button2);
              write.setOnClickListener(list2);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.activity_main, menu);
        return true;
    }

}
