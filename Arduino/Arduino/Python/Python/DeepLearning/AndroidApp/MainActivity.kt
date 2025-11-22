package com.example.eegapp

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothSocket
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import java.io.InputStream
import java.util.UUID
import kotlin.concurrent.thread

class MainActivity : AppCompatActivity() {
    lateinit var btSocket: BluetoothSocket
    lateinit var input: InputStream

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val eegText = findViewById<TextView>(R.id.eegText)

        val adapter = BluetoothAdapter.getDefaultAdapter()
        val device = adapter.getRemoteDevice("00:11:22:33:44:55") // replace with your MAC
        val uuid = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb")

        thread {
            try {
                btSocket = device.createRfcommSocketToServiceRecord(uuid)
                btSocket.connect()
                input = btSocket.inputStream
                val buffer = ByteArray(1024)
                while (true) {
                    val bytes = input.read(buffer)
                    val data = String(buffer, 0, bytes)
                    runOnUiThread { eegText.text = data }
                }
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
    }
}
