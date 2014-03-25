using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;

namespace LoPAnalyze
{
    public class LoPNode
    {
        private String serialPortName;

        public String SerialPortName
        {
            get { return serialPortName; }
            set { serialPortName = value; }
        }

        SerialPort serialPort = null;
        
        private bool ensureConnection()
        {
            try
            {
                if (serialPort == null)
                {
                    serialPort = new SerialPort(serialPortName);
                    serialPort.Open();
                    serialPort.BaudRate = 115200;
                    serialPort.ReadTimeout = 500;
                    serialPort.NewLine = "\n";
                }
                else
                {
                    if (!serialPort.IsOpen)
                    {
                        serialPort.Open();
                    }
                }
                return serialPort.IsOpen;
            }
            catch(Exception e)
            {
                return false;
            }
        }

        private void sendCommand(String command)
        {
            if(ensureConnection())
            {
                serialPort.WriteLine(command);
                waitOK();
            }
        }

        private String sendQuery(String command)
        {
            if (ensureConnection())
            {
                serialPort.WriteLine(command);
                return readResponse();
            }

            return null;
        }

        private String readResponse()
        {
            if (ensureConnection())
            {
                String response = "";
                while (response == "" || response.StartsWith("OK"))
                {
                    response = serialPort.ReadLine();
                    response = response.Replace("\n", "").Replace("\r", "");
                }
                return response;
            }

            return null;
        }

        private void waitOK()
        {
            if (ensureConnection())
            {
                String response = "";
                while (!response.Contains("OK"))
                {
                    response = serialPort.ReadLine();
                    response = response.Replace("\n", "").Replace("\r", "");
                }
            }
        }

        public byte ReadConfiguration(UInt16 address)
        {
            String response = sendQuery("ATCFGR " + address.ToString());
            if(!String.IsNullOrEmpty(response))
            {
                return byte.Parse(response);
            }

            return 0;
        }

        public List<byte> ReadConfiguration(UInt16 address, int amount)
        {
            String response = sendQuery("ATCFGR " + address.ToString() + " " + amount.ToString());
            if (!String.IsNullOrEmpty(response))
            {
                List<byte> res = new List<byte>();
                foreach (String value in response.Split(' '))
                {
                    if (value != "")
                    {
                        res.Add(byte.Parse(value));
                    }
                }
                return res;
            }

            return null;
        }

        public void WriteConfiguration(int address, List<Byte> values)
        {
            foreach(byte value in values)
            {
                sendCommand("ATCFGW " + address.ToString() + " " + value.ToString());
                address++;
            }
        }
    }
}
