import java.io.BufferedReader;
import java.io.Console;
import java.io.IOException;
import java.io.InputStreamReader;

public class Main {
    public static void main(String[] args) throws Exception {
        if (args.length < 1)
            throw new IllegalArgumentException("Wrong number of args, you have specify which cluster you want to join" +
                    ", and client name.");
        String instanceName = args[0];
        System.out.println(instanceName);
        System.setProperty("java.net.preferIPv4Stack", "true");

        try {
            DistributedMap map = new DistributedMap("mapCluster", instanceName);

            while (true) {
                String command = getUserInput();

                String arr[] = command.split(" ");

                if (command.equals("exit")) {
                    break;
                }
                switch (arr[0]) {
                    case "contains":
                        if (map.containsKey(arr[1])) {
                            System.out.println("Map contains key " + arr[1]);
                        } else {
                            System.out.println("Map does not contain key " + arr[1]);
                        }
                        break;
                    case "get":

                        if (map.get(arr[1]) != null) {
                            System.out.println("Value at key: " + arr[1] + " is: " + map.get(arr[1]));
                        } else {
                            System.out.println("There is no value under that key.");
                        }

                        break;
                    case "remove":
                        map.remove(arr[1]);
                        break;
                    case "put":
                        map.put(arr[1], Integer.parseInt(arr[2]));
                        break;
                    case "show":
                        map.showMap();
                        break;
                    case "":
                        break;
                    default: {
                        System.out.println("Unrecognised command.");
                        break;
                    }

                }
            }
            map.close();
        } catch (Exception e) {
            e.printStackTrace();
            throw new Exception("Unhandled Exception happened during execution.");
        }

    }

    public static String getUserInput() {
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        String content = null;
        try {
            content = in.readLine();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return content;
    }

}
