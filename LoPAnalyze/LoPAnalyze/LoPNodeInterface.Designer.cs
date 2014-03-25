namespace LoPAnalyze
{
    partial class LoPNodeInterface
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LoPNodeInterface));
            this.textBoxConfigBytes = new System.Windows.Forms.TextBox();
            this.contextMenuConfigBytes = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.reloadConfigBytesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.label3 = new System.Windows.Forms.Label();
            this.listViewCron = new System.Windows.Forms.ListView();
            this.columnHeaderHours = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeaderMinutes = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeaderSeconds = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeaderEvent = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.textBoxNID7 = new System.Windows.Forms.TextBox();
            this.textBoxNID6 = new System.Windows.Forms.TextBox();
            this.textBoxNID5 = new System.Windows.Forms.TextBox();
            this.textBoxNID4 = new System.Windows.Forms.TextBox();
            this.textBoxNID3 = new System.Windows.Forms.TextBox();
            this.textBoxNID2 = new System.Windows.Forms.TextBox();
            this.textBoxNID1 = new System.Windows.Forms.TextBox();
            this.textBoxNID0 = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.textBoxUID = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tabPageRawConfig = new System.Windows.Forms.TabPage();
            this.contextMenuConfigBytes.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabPageRawConfig.SuspendLayout();
            this.SuspendLayout();
            // 
            // textBoxConfigBytes
            // 
            this.textBoxConfigBytes.ContextMenuStrip = this.contextMenuConfigBytes;
            this.textBoxConfigBytes.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBoxConfigBytes.Font = new System.Drawing.Font("Courier New", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBoxConfigBytes.Location = new System.Drawing.Point(3, 3);
            this.textBoxConfigBytes.Multiline = true;
            this.textBoxConfigBytes.Name = "textBoxConfigBytes";
            this.textBoxConfigBytes.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBoxConfigBytes.Size = new System.Drawing.Size(946, 894);
            this.textBoxConfigBytes.TabIndex = 0;
            // 
            // contextMenuConfigBytes
            // 
            this.contextMenuConfigBytes.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.reloadConfigBytesToolStripMenuItem});
            this.contextMenuConfigBytes.Name = "contextMenuConfigBytes";
            this.contextMenuConfigBytes.Size = new System.Drawing.Size(171, 34);
            // 
            // reloadConfigBytesToolStripMenuItem
            // 
            this.reloadConfigBytesToolStripMenuItem.Name = "reloadConfigBytesToolStripMenuItem";
            this.reloadConfigBytesToolStripMenuItem.ShortcutKeys = System.Windows.Forms.Keys.F5;
            this.reloadConfigBytesToolStripMenuItem.Size = new System.Drawing.Size(170, 30);
            this.reloadConfigBytesToolStripMenuItem.Text = "Reload";
            this.reloadConfigBytesToolStripMenuItem.Click += new System.EventHandler(this.reloadConfigBytesToolStripMenuItem_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPageRawConfig);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(960, 933);
            this.tabControl1.TabIndex = 1;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.label3);
            this.tabPage2.Controls.Add(this.listViewCron);
            this.tabPage2.Controls.Add(this.textBoxNID7);
            this.tabPage2.Controls.Add(this.textBoxNID6);
            this.tabPage2.Controls.Add(this.textBoxNID5);
            this.tabPage2.Controls.Add(this.textBoxNID4);
            this.tabPage2.Controls.Add(this.textBoxNID3);
            this.tabPage2.Controls.Add(this.textBoxNID2);
            this.tabPage2.Controls.Add(this.textBoxNID1);
            this.tabPage2.Controls.Add(this.textBoxNID0);
            this.tabPage2.Controls.Add(this.label2);
            this.tabPage2.Controls.Add(this.textBoxUID);
            this.tabPage2.Controls.Add(this.label1);
            this.tabPage2.Location = new System.Drawing.Point(4, 29);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(952, 900);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Smart Node";
            this.tabPage2.UseVisualStyleBackColor = true;
            this.tabPage2.Click += new System.EventHandler(this.tabPage2_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(34, 135);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(47, 20);
            this.label3.TabIndex = 12;
            this.label3.Text = "Cron:";
            // 
            // listViewCron
            // 
            this.listViewCron.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeaderHours,
            this.columnHeaderMinutes,
            this.columnHeaderSeconds,
            this.columnHeaderEvent});
            this.listViewCron.Location = new System.Drawing.Point(143, 135);
            this.listViewCron.Name = "listViewCron";
            this.listViewCron.Size = new System.Drawing.Size(458, 241);
            this.listViewCron.TabIndex = 11;
            this.listViewCron.UseCompatibleStateImageBehavior = false;
            this.listViewCron.View = System.Windows.Forms.View.Details;
            // 
            // columnHeaderHours
            // 
            this.columnHeaderHours.Text = "H";
            // 
            // columnHeaderMinutes
            // 
            this.columnHeaderMinutes.Text = "M";
            // 
            // columnHeaderSeconds
            // 
            this.columnHeaderSeconds.Text = "S";
            // 
            // columnHeaderEvent
            // 
            this.columnHeaderEvent.Text = "Event";
            this.columnHeaderEvent.Width = 250;
            // 
            // textBoxNID7
            // 
            this.textBoxNID7.Location = new System.Drawing.Point(549, 60);
            this.textBoxNID7.Name = "textBoxNID7";
            this.textBoxNID7.Size = new System.Drawing.Size(52, 26);
            this.textBoxNID7.TabIndex = 10;
            this.textBoxNID7.TextChanged += new System.EventHandler(this.inputControl_TextChanged);
            this.textBoxNID7.Leave += new System.EventHandler(this.inputControl_Leave);
            // 
            // textBoxNID6
            // 
            this.textBoxNID6.Location = new System.Drawing.Point(491, 60);
            this.textBoxNID6.Name = "textBoxNID6";
            this.textBoxNID6.Size = new System.Drawing.Size(52, 26);
            this.textBoxNID6.TabIndex = 9;
            this.textBoxNID6.TextChanged += new System.EventHandler(this.inputControl_TextChanged);
            this.textBoxNID6.Leave += new System.EventHandler(this.inputControl_Leave);
            // 
            // textBoxNID5
            // 
            this.textBoxNID5.Location = new System.Drawing.Point(433, 60);
            this.textBoxNID5.Name = "textBoxNID5";
            this.textBoxNID5.Size = new System.Drawing.Size(52, 26);
            this.textBoxNID5.TabIndex = 8;
            this.textBoxNID5.TextChanged += new System.EventHandler(this.inputControl_TextChanged);
            this.textBoxNID5.Leave += new System.EventHandler(this.inputControl_Leave);
            // 
            // textBoxNID4
            // 
            this.textBoxNID4.Location = new System.Drawing.Point(375, 60);
            this.textBoxNID4.Name = "textBoxNID4";
            this.textBoxNID4.Size = new System.Drawing.Size(52, 26);
            this.textBoxNID4.TabIndex = 7;
            this.textBoxNID4.TextChanged += new System.EventHandler(this.inputControl_TextChanged);
            this.textBoxNID4.Leave += new System.EventHandler(this.inputControl_Leave);
            // 
            // textBoxNID3
            // 
            this.textBoxNID3.Location = new System.Drawing.Point(317, 60);
            this.textBoxNID3.Name = "textBoxNID3";
            this.textBoxNID3.Size = new System.Drawing.Size(52, 26);
            this.textBoxNID3.TabIndex = 6;
            this.textBoxNID3.TextChanged += new System.EventHandler(this.inputControl_TextChanged);
            this.textBoxNID3.Leave += new System.EventHandler(this.inputControl_Leave);
            // 
            // textBoxNID2
            // 
            this.textBoxNID2.Location = new System.Drawing.Point(259, 60);
            this.textBoxNID2.Name = "textBoxNID2";
            this.textBoxNID2.Size = new System.Drawing.Size(52, 26);
            this.textBoxNID2.TabIndex = 5;
            this.textBoxNID2.TextChanged += new System.EventHandler(this.inputControl_TextChanged);
            this.textBoxNID2.Leave += new System.EventHandler(this.inputControl_Leave);
            // 
            // textBoxNID1
            // 
            this.textBoxNID1.Location = new System.Drawing.Point(201, 60);
            this.textBoxNID1.Name = "textBoxNID1";
            this.textBoxNID1.Size = new System.Drawing.Size(52, 26);
            this.textBoxNID1.TabIndex = 4;
            this.textBoxNID1.TextChanged += new System.EventHandler(this.inputControl_TextChanged);
            this.textBoxNID1.Leave += new System.EventHandler(this.inputControl_Leave);
            // 
            // textBoxNID0
            // 
            this.textBoxNID0.Location = new System.Drawing.Point(143, 60);
            this.textBoxNID0.Name = "textBoxNID0";
            this.textBoxNID0.Size = new System.Drawing.Size(52, 26);
            this.textBoxNID0.TabIndex = 3;
            this.textBoxNID0.TextChanged += new System.EventHandler(this.inputControl_TextChanged);
            this.textBoxNID0.Leave += new System.EventHandler(this.inputControl_Leave);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(34, 63);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(41, 20);
            this.label2.TabIndex = 2;
            this.label2.Text = "NID:";
            // 
            // textBoxUID
            // 
            this.textBoxUID.Location = new System.Drawing.Point(143, 92);
            this.textBoxUID.Name = "textBoxUID";
            this.textBoxUID.Size = new System.Drawing.Size(458, 26);
            this.textBoxUID.TabIndex = 1;
            this.textBoxUID.TextChanged += new System.EventHandler(this.inputControl_TextChanged);
            this.textBoxUID.Leave += new System.EventHandler(this.inputControl_Leave);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(34, 95);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(103, 20);
            this.label1.TabIndex = 0;
            this.label1.Text = "msgp2p UID:";
            // 
            // tabPageRawConfig
            // 
            this.tabPageRawConfig.Controls.Add(this.textBoxConfigBytes);
            this.tabPageRawConfig.Location = new System.Drawing.Point(4, 29);
            this.tabPageRawConfig.Name = "tabPageRawConfig";
            this.tabPageRawConfig.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageRawConfig.Size = new System.Drawing.Size(952, 900);
            this.tabPageRawConfig.TabIndex = 0;
            this.tabPageRawConfig.Text = "Raw Config";
            this.tabPageRawConfig.UseVisualStyleBackColor = true;
            // 
            // LoPNodeInterface
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(960, 933);
            this.ControlBox = false;
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "LoPNodeInterface";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "LoPNodeInterface";
            this.Load += new System.EventHandler(this.LoPNodeInterface_Load);
            this.contextMenuConfigBytes.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.tabPageRawConfig.ResumeLayout(false);
            this.tabPageRawConfig.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox textBoxConfigBytes;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPageRawConfig;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.TextBox textBoxUID;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBoxNID0;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBoxNID7;
        private System.Windows.Forms.TextBox textBoxNID6;
        private System.Windows.Forms.TextBox textBoxNID5;
        private System.Windows.Forms.TextBox textBoxNID4;
        private System.Windows.Forms.TextBox textBoxNID3;
        private System.Windows.Forms.TextBox textBoxNID2;
        private System.Windows.Forms.TextBox textBoxNID1;
        private System.Windows.Forms.ContextMenuStrip contextMenuConfigBytes;
        private System.Windows.Forms.ToolStripMenuItem reloadConfigBytesToolStripMenuItem;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ListView listViewCron;
        private System.Windows.Forms.ColumnHeader columnHeaderHours;
        private System.Windows.Forms.ColumnHeader columnHeaderMinutes;
        private System.Windows.Forms.ColumnHeader columnHeaderSeconds;
        private System.Windows.Forms.ColumnHeader columnHeaderEvent;
    }
}