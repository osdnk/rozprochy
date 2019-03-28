import org.jgroups.JChannel;
import org.jgroups.Message;
import org.jgroups.Receiver;
import org.jgroups.ReceiverAdapter;
import org.jgroups.protocols.pbcast.GMS;
import org.jgroups.protocols.pbcast.NAKACK2;
import org.jgroups.protocols.pbcast.STABLE;
import org.jgroups.protocols.pbcast.STATE_TRANSFER;
import org.jgroups.stack.ProtocolStack;
import org.jgroups.protocols.*;
import org.jgroups.util.Util;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashMap;

public class DistributedMap extends ReceiverAdapter implements SimpleStringMap {
    private JChannel channel;
    private final HashMap<String, Integer> map = new HashMap<>();

    public DistributedMap(String clusterName, String clientName) throws Exception {
        startClient();
        this.channel.setName(clientName);
        //set this object (extension of ReceiverAdapter), as receiver of this channel's messages.
        this.channel.setReceiver(this);
        this.channel.connect(clusterName);
        //get actual state of the map
        this.channel.getState(null, 10000);

    }

    private void startClient() throws Exception {
        this.channel = new JChannel(false);
        ProtocolStack stack = new ProtocolStack();
        this.channel.setProtocolStack(stack);
        stack.addProtocol(new UDP())
                .addProtocol(new PING())
                .addProtocol(new MERGE3())
                .addProtocol(new FD_SOCK())
                .addProtocol(new FD_ALL()
                        .setValue("timeout", 12000)
                        .setValue("interval", 3000))
                .addProtocol(new VERIFY_SUSPECT())
                .addProtocol(new BARRIER())
                .addProtocol(new NAKACK2())
                .addProtocol(new UNICAST3())
                .addProtocol(new STABLE())
                .addProtocol(new GMS())
                .addProtocol(new UFC())
                .addProtocol(new MFC())
                .addProtocol(new FRAG2())
                .addProtocol(new STATE_TRANSFER());
        stack.init();

    }


    @Override
    public boolean containsKey(String key) {
        return map.containsKey(key);
    }

    @Override
    public Integer get(String key) {
        return map.get(key);
    }

    @Override
    public void put(String key, Integer value) {
        //Procedure which needs synchronizing state
        try {
            send("put " + key + " " + value.toString());
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Exception while putting something to the map");
        }
    }

    @Override
    public Integer remove(String key) {
        try {
            send("rmv " + key);
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Exception while removing something from the map");
            return null;
        }
        return map.remove(key);
    }

    @Override
    public void receive(Message msg) {
        String payload = msg.getObject().toString();

        String arr[] = payload.split(" ");
        switch (arr[0]) {
            case "rmv": {
                map.remove(arr[1]);
                System.out.println("Received remove request at key: " + arr[1]);
                break;
            }
            case "put": {
                map.put(arr[1], Integer.parseInt(arr[2]));
                System.out.println("Received put request at key: " + arr[1] + " with value: " + arr[2]);
                break;
            }
            default:
                System.out.println("Unrecognised message: " + payload);
                break;
        }
    }

    public void send(String message) throws Exception {
        Message msg = new Message(null, null, message);
        try {
            channel.send(msg);
        } catch (Exception e) {
            throw new Exception("Exception happens during message send.");
        }
    }

    public void getState(OutputStream output) throws Exception {
        synchronized (map) {
            Util.objectToStream(map, new DataOutputStream(output));
        }

    }

    public void setState(InputStream input) throws Exception {
        HashMap<String, Integer> inputMap;
        inputMap = (HashMap<String, Integer>) Util.objectFromStream(new DataInputStream(input));
        synchronized (map) {
            //TODO -> do i need this?
            map.clear();
            map.putAll(inputMap);
        }
        System.out.println("Received current state from channel");
        showMap();
    }

    public void showMap() {
        if(!map.isEmpty())
            map.entrySet().forEach(System.out::println);
        else
            System.out.println("<empty map>");
    }

    public void close() {
        channel.close();
    }
}
