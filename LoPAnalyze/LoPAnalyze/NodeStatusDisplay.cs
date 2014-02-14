using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace LoPAnalyze
{
    public partial class NodeStatusDisplay : Form
    {
        public NodeStatusDisplay()
        {
            InitializeComponent();
        }

        private void NodeStatusDisplay_Load(object sender, EventArgs e)
        {

        }

        public String SerialPortName { get; set; }

        private SerialPort serialPort = null;

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (!String.IsNullOrEmpty(SerialPortName))
            {
                try
                {
                    if (serialPort == null)
                    {
                        serialPort = new SerialPort(SerialPortName);
                        serialPort.BaudRate = 115200;
                        serialPort.ReadTimeout = 500;
                        serialPort.NewLine = "\n";
                        serialPort.Open();
                    }
                    try
                    {

                        serialPort.ReadExisting();
                        serialPort.WriteLine("ATID?");
                        String response = serialPort.ReadLine();
                        this.Text = response.Replace("\n", "").Replace("\r", "") + " (" + SerialPortName + ")";

                        serialPort.ReadExisting();
                        serialPort.WriteLine("ATDAP?");
                        response = serialPort.ReadLine();
                        labelDAP.Text = response.Replace("\n", "").Replace("\r", "");

                        serialPort.ReadExisting();
                        serialPort.WriteLine("ATADD?");
                        response = serialPort.ReadLine();
                        labelAddress.Text = response.Replace("\n", "").Replace("\r", "");

                        listViewONL.Items.Clear();
                        serialPort.ReadExisting();
                        serialPort.WriteLine("ATONL?");
                        response = "";
                        while (!response.Contains("OK"))
                        {
                            response = serialPort.ReadLine();
                            if (!response.Contains("OK"))
                            {
                                List<String> onlTokens = new List<String>(response.Replace("\n", "").Split(','));
                                ListViewItem item = listViewONL.Items.Add(onlTokens[1]);
                                if (int.Parse(onlTokens[2]) > 9000)
                                {
                                    item.SubItems.Add(((3 - int.Parse(onlTokens[0])) * -6).ToString() + " dBm");
                                    item.SubItems.Add(onlTokens[2]);
                                    item.SubItems.Add(onlTokens[3]);
                                    item.SubItems.Add("Active");
                                }
                                else if (int.Parse(onlTokens[2]) > 0)
                                {
                                    item.SubItems.Add("---");
                                    item.SubItems.Add(onlTokens[2]);
                                    item.SubItems.Add(onlTokens[3]);
                                    item.SubItems.Add("Time Wait");
                                }
                                else
                                {
                                    item.SubItems.Add("---");
                                    item.SubItems.Add("---");
                                    item.SubItems.Add("---");
                                    item.SubItems.Add("Free");
                                }

                            }
                        }
                    }
                    catch (TimeoutException)
                    {
                        // Port not ready yet, let's keep it to monitor.
                    }
                }
                catch (Exception)
                {
                    if (serialPort != null)
                    {
                        if (serialPort.IsOpen)
                        {
                            serialPort.Close();
                        }
                        serialPort = null;
                        this.Text = "Device Offline (" + SerialPortName + ")";
                        listViewONL.Items.Clear();
                    }
                }
            }
        }

    }
}
