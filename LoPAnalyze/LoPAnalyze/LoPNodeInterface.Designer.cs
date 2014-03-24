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
            this.textBoxConfigBytes = new System.Windows.Forms.TextBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPageRawConfig = new System.Windows.Forms.TabPage();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.tabControl1.SuspendLayout();
            this.tabPageRawConfig.SuspendLayout();
            this.SuspendLayout();
            // 
            // textBoxConfigBytes
            // 
            this.textBoxConfigBytes.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBoxConfigBytes.Font = new System.Drawing.Font("Courier New", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBoxConfigBytes.Location = new System.Drawing.Point(3, 3);
            this.textBoxConfigBytes.Multiline = true;
            this.textBoxConfigBytes.Name = "textBoxConfigBytes";
            this.textBoxConfigBytes.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBoxConfigBytes.Size = new System.Drawing.Size(946, 894);
            this.textBoxConfigBytes.TabIndex = 0;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPageRawConfig);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(960, 933);
            this.tabControl1.TabIndex = 1;
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
            // tabPage2
            // 
            this.tabPage2.Location = new System.Drawing.Point(4, 29);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(952, 900);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "tabPage2";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // LoPNodeInterface
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(960, 933);
            this.Controls.Add(this.tabControl1);
            this.Name = "LoPNodeInterface";
            this.Text = "LoPNodeInterface";
            this.Load += new System.EventHandler(this.LoPNodeInterface_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabPageRawConfig.ResumeLayout(false);
            this.tabPageRawConfig.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox textBoxConfigBytes;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPageRawConfig;
        private System.Windows.Forms.TabPage tabPage2;
    }
}