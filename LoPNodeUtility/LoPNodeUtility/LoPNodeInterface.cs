// LoPNodeInterface is part of LoP-RAN , provides GUI control to configure a node.
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
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using LoPNodeUtility;

namespace LoPNodeUtilityGUI
{
    /// <summary>
    /// A form to interact with the node and its internal configuration.
    /// </summary>
    public partial class LoPNodeInterface : Form
    {
        /// <summary>
        /// The node associated with this form.
        /// </summary>
        private LoPNode lopNode;

        // Here we store Actions to be run whenever a modified control loses focus.
        private Dictionary<Control, Action> inputControlsUpdateFunctions = new Dictionary<Control, Action>();

        /// <summary>
        /// 
        /// </summary>
        public LoPNodeInterface()
        {
            InitializeComponent();

            // When leaving a control update functions are run if the control content has changed. To keep the logic
            //  in a common place we store in this dictionary the update functions related to each control.
            inputControlsUpdateFunctions.Add(textBoxmsgp2pUID, (Action)(() => { this.writemsgp2pUID(); }));
            inputControlsUpdateFunctions.Add(textBoxNID0, (Action)(() => { this.writeNID(textBoxNID0, 0); }));
            inputControlsUpdateFunctions.Add(textBoxNID1, (Action)(() => { this.writeNID(textBoxNID1, 1); }));
            inputControlsUpdateFunctions.Add(textBoxNID2, (Action)(() => { this.writeNID(textBoxNID2, 2); }));
            inputControlsUpdateFunctions.Add(textBoxNID3, (Action)(() => { this.writeNID(textBoxNID3, 3); }));
            inputControlsUpdateFunctions.Add(textBoxNID4, (Action)(() => { this.writeNID(textBoxNID4, 4); }));
            inputControlsUpdateFunctions.Add(textBoxNID5, (Action)(() => { this.writeNID(textBoxNID5, 5); }));
            inputControlsUpdateFunctions.Add(textBoxNID6, (Action)(() => { this.writeNID(textBoxNID6, 6); }));
            inputControlsUpdateFunctions.Add(textBoxNID7, (Action)(() => { this.writeNID(textBoxNID7, 7); }));

            // Prevent form resizing even when double clicking the top bar.
            this.MinimumSize = this.Size;
            this.MaximumSize = this.Size;

        }

        /// <summary>
        /// Gets or sets the node associated with this interface.
        /// </summary>
        public LoPNode LoPNode
        {
            get { return lopNode; }
            set
            {
                lopNode = value;

                fetchmsgp2pUID();
                fetchNID();
                fetchCron();
            }
        }

        /// <summary>
        /// Fetches the all configuraton EEPROM content. Since this operation is lengthy
        ///     we avoid running it in the GUI thread and we append to GUI every time a 
        ///     new chunck is ready so GUI stays responsive and user has a constant 
        ///     feedback on what is happening.
        /// </summary>
        private void fetchConfigBytes()
        {
            // IF we are in GUI thread run ourselves in another thread.
            if (!this.InvokeRequired)
            {
                Thread backgroundWorker = new Thread(fetchConfigBytes);
                backgroundWorker.Start();
                return;
            }

            // Since here we are surely out of GUI thread we need to invoke for GUI operations.
            // We format data in the classic HEX dump format, 16 bytes per line with address at  the 
            //  beginning and ASCII dump at the end of the line.

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

        /// <summary>
        /// Fetches the msgp2p UID currently programmed in the node. 
        /// </summary>
        private void fetchmsgp2pUID()
        {
            List<Byte> value = lopNode.ReadConfiguration(11, 40);
            String uid = System.Text.ASCIIEncoding.Default.GetString(value.ToArray<Byte>());
            textBoxmsgp2pUID.Text = uid;
            textBoxmsgp2pUID.Tag = false;
        }

        /// <summary>
        /// Writes the msgp2p UID to the node.
        /// </summary>
        private void writemsgp2pUID()
        {
            List<byte> values = new List<Byte>(System.Text.ASCIIEncoding.Default.GetBytes(textBoxmsgp2pUID.Text));
            while (values.Count < 40)
            {
                values.Add(0);
            }
            lopNode.WriteConfiguration(11, values);
        }

        /// <summary>
        /// Fetches the NID (Network Identifier) that is currently programmed in the node.
        /// </summary>
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

        /// <summary>
        /// Writes the NID (Network Identifier) to the node.
        /// </summary>
        /// <param name="nidTextBox"></param>
        /// <param name="nidPosition"></param>
        private void writeNID(TextBox nidTextBox, int nidPosition)
        {
            byte value = byte.Parse(nidTextBox.Text);
            lopNode.WriteConfiguration(3 + nidPosition, value);
        }

        /// <summary>
        /// Fetches the current cron schedule from the node.
        /// </summary>
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

        /// <summary>
        /// Helper function to convert a cron value to a string.
        /// Cron values represent one of the following:
        ///     -  Not in use
        ///     *   Any value
        ///     n   At n (minute, hour, second)
        ///     /n  Every n (minutes, hours, seconds)
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        private String cronValueToString(byte value)
        {
            String prefix = "";
            if (value == 0xFF)
            {
                // 0xFF indicates not in use.
                return "-";
            }
            else if ((value & 1 << 7) != 0)
            {
                // Bit7 indicates * (any value)
                return "*";
            }
            else if ((value & 1 << 6) != 0)
            {
                // Bit6 indicates / (frequency of repetition)
                prefix = "/";
            }

            // Lower bits are the actual value.
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

        /// <summary>
        /// User modifide a value. We signal this by storing a bool true in the tag.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void inputControl_TextChanged(object sender, EventArgs e)
        {
            Control senderControl = sender as Control;
            senderControl.Tag = true;
        }

        /// <summary>
        /// This is the config bytes view contextual menu to reload the view content.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void reloadConfigBytesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            fetchConfigBytes();
        }


    }
}
