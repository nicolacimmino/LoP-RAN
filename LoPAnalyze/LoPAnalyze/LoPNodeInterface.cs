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
            inputControlsUpdateFunctions.Add(textBoxUID, (Action)(() => { this.updateUID(); }));
            inputControlsUpdateFunctions.Add(textBoxNID0, (Action)(() => { this.updateNID(textBoxNID0, 0); }));
            inputControlsUpdateFunctions.Add(textBoxNID1, (Action)(() => { this.updateNID(textBoxNID1, 1); }));
            inputControlsUpdateFunctions.Add(textBoxNID2, (Action)(() => { this.updateNID(textBoxNID2, 2); }));
            inputControlsUpdateFunctions.Add(textBoxNID3, (Action)(() => { this.updateNID(textBoxNID3, 3); }));
            inputControlsUpdateFunctions.Add(textBoxNID4, (Action)(() => { this.updateNID(textBoxNID4, 4); }));
            inputControlsUpdateFunctions.Add(textBoxNID5, (Action)(() => { this.updateNID(textBoxNID5, 5); }));
            inputControlsUpdateFunctions.Add(textBoxNID6, (Action)(() => { this.updateNID(textBoxNID6, 6); }));
            inputControlsUpdateFunctions.Add(textBoxNID7, (Action)(() => { this.updateNID(textBoxNID7, 7); }));
            this.MinimumSize = this.Size;
            this.MaximumSize = this.Size;

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
                //refreshConfigBytes();
                fetchUID();
                fetchNID();
                fetchCron();
            }
        }


        private void fetchConfigBytes()
        {
            if (!this.InvokeRequired)
            {
                Thread backgroundWorker = new Thread(fetchConfigBytes);
                backgroundWorker.Start();
                return;
            }

            this.Invoke((MethodInvoker)(() => { textBoxConfigBytes.Text = ""; }));
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

        private void tabPage2_Click(object sender, EventArgs e)
        {

        }

        private void fetchUID()
        {
            List<Byte> value = lopNode.ReadConfiguration(11, 40);
            String uid = System.Text.ASCIIEncoding.Default.GetString(value.ToArray<Byte>());
            textBoxUID.Text = uid;
            textBoxUID.Tag = false;
        }

        private void updateUID()
        {
            List<byte> values = new List<Byte>(System.Text.ASCIIEncoding.Default.GetBytes(textBoxUID.Text));
            while (values.Count < 40)
            {
                values.Add(0);
            }
            lopNode.WriteConfiguration(11, values);
        }

        private void fetchNID()
        {
            List<Byte> values = lopNode.ReadConfiguration(3, 8);
            textBoxNID0.Text = values[0].ToString();
            textBoxNID1.Text = values[1].ToString();
            textBoxNID2.Text = values[2].ToString();
            textBoxNID3.Text = values[3].ToString();
            textBoxNID4.Text = values[4].ToString();
            textBoxNID5.Text = values[5].ToString();
            textBoxNID6.Text = values[6].ToString();
            textBoxNID7.Text = values[7].ToString();
            textBoxNID0.Tag = false;
            textBoxNID1.Tag = false;
            textBoxNID2.Tag = false;
            textBoxNID3.Tag = false;
            textBoxNID4.Tag = false;
            textBoxNID5.Tag = false;
            textBoxNID6.Tag = false;
            textBoxNID7.Tag = false;
        }

        private void updateNID(TextBox nidTextBox, int nidPosition)
        {
            List<byte> values = new List<Byte>();
            values.Add(byte.Parse(nidTextBox.Text));
            lopNode.WriteConfiguration(3 + nidPosition, values);
        }

        private void fetchCron()
        {
            List<Byte> values = lopNode.ReadConfiguration(0x200, 0x80);
            listViewCron.Items.Clear();
            ListViewItem newItem = listViewCron.Items.Add(cronValueToString(values[0]));
            if (values[0] != 0xFF)
            {
                newItem.SubItems.Add(cronValueToString(values[1]));
                newItem.SubItems.Add(cronValueToString(values[2]));
                values.RemoveRange(0, 3);
                newItem.SubItems.Add(System.Text.ASCIIEncoding.Default.GetString(values.ToArray<Byte>()));
            }
        }

        private String cronValueToString(byte value)
        {
            String prefix = "";
            if (value == 0xFF)
            {
                return "--";
            }
            else if ((value & 1 << 7) != 0)
            {
                prefix = "*";
            }
            else if ((value & 1 << 6) != 0)
            {
                prefix = "/";
            }

            return prefix + (value & 63).ToString();

        }

        /// <summary>
        /// This will be invoked by the leave of any input control. If an update handle function
        ///     is associated with it and the content has changed since the last update it will
        ///     trigger the update function.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void inputControl_Leave(object sender, EventArgs e)
        {
            Control senderControl = sender as Control;
            if (senderControl.Tag as bool? == true)
            {
                this.Cursor = Cursors.WaitCursor;
                inputControlsUpdateFunctions[senderControl].Invoke();
                senderControl.Tag = false;
                this.Cursor = this.DefaultCursor;
            }
        }

        private Dictionary<Control, Action> inputControlsUpdateFunctions = new Dictionary<Control, Action>();

        private void inputControl_TextChanged(object sender, EventArgs e)
        {
            Control senderControl = sender as Control;
            senderControl.Tag = true;
        }

        private void reloadConfigBytesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            fetchConfigBytes();
        }

        
    }
}
