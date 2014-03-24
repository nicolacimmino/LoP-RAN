using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;

namespace LoPAnalyze
{
    public partial class LoPNodeInterface : Form
    {
        public LoPNodeInterface()
        {
            InitializeComponent();
        }

        private void LoPNodeInterface_Load(object sender, EventArgs e)
        {

        }

        private LoPNode lopNode;

        public LoPNode LoPNode
        {
            get { return lopNode; }
            set
            {
                lopNode = value;
                refreshConfigBytes();
            }
        }


        private void refreshConfigBytes()
        {
            if(!this.InvokeRequired)
            {
                Thread backgroundWorker = new Thread(refreshConfigBytes);
                backgroundWorker.Start();
                return;
            }
            textBoxConfigBytes.Text = "";
            for (UInt16 add = 0; add < 1024; add += 16)
            {
                String dump = add.ToString("X4") + " - ";
                String ascii = "";
                foreach (byte value in lopNode.ReadConfiguration(add, 16))
                {
                    dump += value.ToString("X2") + ".";
                    if (value >= 32 && value < 128)
                    {
                        ascii += char.ConvertFromUtf32(value);
                    }
                    else
                    {
                        ascii += ".";
                    }

                }
                dump += ascii + "\r\n";
                this.Invoke((MethodInvoker)(() => { textBoxConfigBytes.Text += dump; }));
            }
        }
    }
}
