﻿// NodesMonitor is part of LoP-RAN , provides discovery and monitor of node connected trough the
//  control interface.
//
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
using System.Threading;
using System.IO.Ports;

namespace LoPNodeUtility
{
    /// <summary>
    /// This class is abstract as we don't want multiple instances monitoring ports.
    /// To start just call the static method NodesMonitor.StartMonitoring() after registering
    ///     to the NewNodeFound event.
    /// </summary>
    abstract class NodesMonitor
    {
        /// <summary>
        /// This is the thread in which the monitoring happens 
        /// continually in bacground.
        /// </summary>
        private static Thread monitoringThread = null;

        /// <summary>
        /// Ports for which we have already found if they are connected to node or not.
        /// </summary>
        private static List<string> knownPorts = new List<string>();

        /// <summary>
        /// Starts monitoring for new nodes being connected.
        /// </summary>
        public static void StartMonitoring()
        {
            if(NodesMonitor.monitoringThread == null)
            {
                NodesMonitor.monitoringThread = new Thread(monitoringLoop);
                monitoringThread.Start();
            }
        }

        /// <summary>
        /// Stops monitoring for new nodes being connected.
        /// </summary>
        public static void StopMonitoring()
        {
            if(NodesMonitor.monitoringThread != null)
            {
                NodesMonitor.monitoringThread.Abort();
                NodesMonitor.monitoringThread = null;
            }
        }

        /// <summary>
        /// This loop runs in its own thread for the all life span of the application.
        /// Every time a new serial port is detected an attempt is made to communicate with it.
        /// If the device turns out to be a LoP-RAN node an event is rasied.
        /// </summary>
        private static void monitoringLoop()
        {
            try
            {
                while (true)
                {
                    String[] serialPortsNames = SerialPort.GetPortNames();
                    foreach (String portName in serialPortsNames)
                    {
                        if (!knownPorts.Contains(portName))
                        {
                            SerialPort port = new SerialPort(portName);
                            port.BaudRate = 115200;
                            port.ReadTimeout = 500;
                            port.NewLine = "\n";
                            try
                            {
                                port.Open();
                                port.WriteTimeout = 500;
                                port.ReadTimeout = 500;
 
                                port.WriteLine("ATID?");
                                try
                                {
                                    String response = port.ReadLine();
                                    if (response.StartsWith("LoP-RAN"))
                                    {
                                        if (NewNodeFound != null)
                                        {
                                            LoPNode lopNode = new LoPNode();
                                            lopNode.SerialPortName = portName;
                                            port.Close();
                                            NewNodeFound(lopNode);
                                        }
                                        knownPorts.Add(portName);
                                    }
                                }
                                catch (TimeoutException)
                                {
                                    // Port not ready yet, let's keep it to monitor.
                                }
                                finally
                                {
                                    port.Close();
                                }
                            }
                            catch(Exception)
                            {
                                // Port might not be ready yet so open throws even if
                                //  the port is listed as available. We don't do anything
                                //  next round the port will likely be ready.
                            }
                        }
                    }

                    // Remove all ports that are in the known list but are not
                    //  anymore in the serial ports, that is these devices have
                    //  been unplugged.
                    knownPorts.RemoveAll(port => !serialPortsNames.Contains(port));

                    Thread.Sleep(1000);
                }
            }
            catch(ThreadAbortException)
            {
                // Thread has been killed, monitoring stopped. We don't have special cleanup to do here for now.
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="lopNode"></param>
        public delegate void NewNodeFoundEventDelegate(LoPNode lopNode);

        /// <summary>
        /// Event informing that a node was found. Provides a ready instance of
        ///     a LoPNode initialized for that node.
        /// </summary>
        public static event NewNodeFoundEventDelegate NewNodeFound;

    }
}
