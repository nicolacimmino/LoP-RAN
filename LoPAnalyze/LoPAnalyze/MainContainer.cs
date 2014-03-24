using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace LoPAnalyze
{
    public partial class MainContainer : Form
    {
        public MainContainer()
        {
            InitializeComponent();
        }

        private void MainContainer_Load(object sender, EventArgs e)
        {
            NodesMonitor.NewNodeFound += NodesMonitor_NewNodeFound;
            NodesMonitor.StartMonitoring();
        }

        void NodesMonitor_NewNodeFound(LoPNode lopNpde)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new Action(() => { NodesMonitor_NewNodeFound(lopNpde); }));
                return;
            }
            if (!statusDisplayForms.Keys.Contains(lopNpde))
            {
                // We don't have yet a form for this port, create it.
                // Otherwise ignore the event as the form will notice the
                //    device coming back online.
                LoPNodeInterface newForm = new LoPNodeInterface();
                statusDisplayForms.Add(lopNpde, newForm);
                newForm.MdiParent = this;
                newForm.Show();
                newForm.LoPNode = lopNpde;

            }
        }

        private void MainContainer_FormClosing(object sender, FormClosingEventArgs e)
        {
            NodesMonitor.StopMonitoring();
        }

        Dictionary<LoPNode, LoPNodeInterface> statusDisplayForms = new Dictionary<LoPNode, LoPNodeInterface>();

    }
}
