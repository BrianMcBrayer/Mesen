﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Mesen.GUI.Config;

namespace Mesen.GUI.Forms.Config
{
	public partial class frmPreferences : BaseConfigForm
	{
		public frmPreferences()
		{
			InitializeComponent();

			Entity = ConfigManager.Config.PreferenceInfo;

			AddBinding("SingleInstance", chkSingleInstance);
			AddBinding("AutoLoadIpsPatches", chkAutoLoadIps);
			AddBinding("AssociateNesFiles", chkNesFormat);

			AddBinding("UseAlternativeMmc3Irq", chkUseAlternativeMmc3Irq);
			AddBinding("AllowInvalidInput", chkAllowInvalidInput);
			AddBinding("RemoveSpriteLimit", chkRemoveSpriteLimit);

			AddBinding("FdsAutoLoadDisk", chkFdsAutoLoadDisk);
			AddBinding("FdsFastForwardOnLoad", chkFdsFastForwardOnLoad);
		}

		protected override void OnFormClosed(FormClosedEventArgs e)
		{
			base.OnFormClosed(e);
			PreferenceInfo.ApplyConfig();
		}
	}
}
