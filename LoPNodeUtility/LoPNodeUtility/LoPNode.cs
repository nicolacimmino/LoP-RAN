// LoPNode is part of LoP-RAN , provides access trough the node control interface.
//  Copyright (C) 2014 Nicola Cimmino
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see http://www.gnu.org/licenses/.
//
// LoP-RAN Specifications are available at https://github.com/nicolacimmino/LoP-RAN/wiki
//    This source code referes, where apllicable, to the chapter and 
//    sub chapter of these documents.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;

namespace LoPNodeUtility
{
    /// <summary>
    /// Represents a single LoP Node connected to this machine.
    /// </summary>
    public class LoPNode
    {
        /// <summary>
        /// The name of the serial port for this node.
        /// </summary>
        private String serialPortName;

        /// <summary>
        /// The serial port for this node.
        /// </summary>
        SerialPort serialPort = null;

        /// <summary>
        /// Gets or sets the name of the serial port for this node.
        /// </summary>
        public String SerialPortName
        {
            get { return serialPortName; }
            set { serialPortName = value; }
        }

        /// <summary>
        /// This must be called before every commmunication with the node,
        ///     ensures that the connection is up and reopens it if it was
        ///     lost. If false is return connection couldn't be open.
        /// </summary>
        /// <returns></returns>
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
                else if (!serialPort.IsOpen)
                {
                    serialPort.Open();
                }

                return serialPort.IsOpen;
            }
            catch (Exception e)
            {
                Console.WriteLine("Exception while opening serial port: " + e.Message);
                return false;
            }
        }

        /// <summary>
        /// Sends command. A command doesn't give any other reply
        /// than "OK".
        /// </summary>
        /// <param name="command"></param>
        private void sendCommand(String command)
        {
            if (ensureConnection())
            {
                serialPort.WriteLine(command);
                waitOK();
            }
        }

        /// <summary>
        /// Sends a query. A query gives any number of reply lines followed by an "OK"
        /// </summary>
        /// <param name="command"></param>
        /// <returns></returns>
        private String sendQuery(String command)
        {
            if (ensureConnection())
            {
                serialPort.WriteLine(command);
                return readResponse();
            }

            return null;
        }

        /// <summary>
        /// Reads the response to a query.
        /// </summary>
        /// <returns></returns>
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

        /// <summary>
        /// Waits until "OK" is received.
        /// </summary>
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

        /// <summary>
        /// Reads a single configuration byte at the specified address.
        /// </summary>
        /// <param name="address"></param>
        /// <returns></returns>
        public byte ReadConfiguration(UInt16 address)
        {
            String response = sendQuery("ATCFGR " + address.ToString());
            if (!String.IsNullOrEmpty(response))
            {
                return byte.Parse(response);
            }

            return 0;
        }

        /// <summary>
        /// Reads multiple configuration bytes starting at the specified address.
        /// </summary>
        /// <param name="address"></param>
        /// <param name="amount"></param>
        /// <returns></returns>
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

        /// <summary>
        /// Writes multiple configuration bytes starting from the supplied address.
        /// </summary>
        /// <param name="address"></param>
        /// <param name="values"></param>
        public void WriteConfiguration(int address, List<Byte> values)
        {
            foreach (byte value in values)
            {
                WriteConfiguration(address, value);
                address++;
            }
        }

        /// <summary>
        /// Writes multiple configuration bytes starting from the supplied address.
        /// </summary>
        /// <param name="address"></param>
        /// <param name="values"></param>
        public void WriteConfiguration(int address, Byte value)
        {
            sendCommand("ATCFGW " + address.ToString() + " " + value.ToString());
        }

    }
}
