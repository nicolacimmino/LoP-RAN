// MainContainer is part of LoP-RAN , provides a container form for the various app forms.
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
using LoPNodeUtility;

namespace LoPNodeUtilityGUI
{
    /// <summary>
    /// An MDI container containing all the application forms.
    /// </summary>
    public partial class MainContainer : Form
    {
        /// <summary>
        /// A collection of all forms currently open and the node they are associated to.
        /// </summary>
        Dictionary<LoPNode, LoPNodeInterface> statusDisplayForms = new Dictionary<LoPNode, LoPNodeInterface>();

        /// <summary>
        /// 
        /// </summary>
        public MainContainer()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MainContainer_Load(object sender, EventArgs e)
        {
            // Start monitoring for new nodes.
            NodesMonitor.NewNodeFound += NodesMonitor_NewNodeFound;
            NodesMonitor.StartMonitoring();
        }

        /// <summary>
        /// A new node has been found.
        /// We need to show a control form for it.
        /// </summary>
        /// <param name="lopNpde"></param>
        void NodesMonitor_NewNodeFound(LoPNode lopNpde)
        {
            // Ensure we are in GUI thread.
            if (this.InvokeRequired)
            {
                this.Invoke(new Action(() => { NodesMonitor_NewNodeFound(lopNpde); }));
                return;
            }

            // We don't have yet a form for this port, create it.
            // Otherwise ignore the event as the form will notice the
            //    device coming back online.
            if (!statusDisplayForms.Keys.Contains(lopNpde))
            {
                LoPNodeInterface newForm = new LoPNodeInterface();
                statusDisplayForms.Add(lopNpde, newForm);
                newForm.MdiParent = this;
                newForm.Show();
                newForm.LoPNode = lopNpde;
            }
        }

        /// <summary>
        /// The application is closing.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MainContainer_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Stop monitoring or the application won't terminate.
            NodesMonitor.StopMonitoring();
        }

    }
}
