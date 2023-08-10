﻿using Ipr.WaterSensor.Core.Entities;
using Ipr.WaterSensor.Infrastructure.Data;
using Ipr.WaterSensor.Server.Services;
using Microsoft.AspNetCore.Components;
using Microsoft.EntityFrameworkCore;
using MQTTnet.Client;
using System;
using System.Text;

namespace Ipr.WaterSensor.Server.Pages
{
    public partial class Index : ComponentBase
    {
        [Inject]
        public MQTTService MQTTService { get; set; } = default!;
        [Inject]
        protected IDbContextFactory<WaterSensorDbContext> DbContextFactory { get; set; } = default!;
        public WaterTank Tank { get; set; } = default!;
        public FireBeetle FireBeetleDevice { get; set; } = default!;
        private async Task GetData()
        {
            using (WaterSensorDbContext context = DbContextFactory.CreateDbContext())
            {
                Tank = await context.WaterTanks.Include(x => x.CurrentWaterLevel).FirstOrDefaultAsync();
                FireBeetleDevice = await context.FireBeetleDevice.FirstOrDefaultAsync();
            }
        }

        private string GetWaterLevelPixels(int percentage)
        {
            var pixels = (210 - (percentage * 2) - 35).ToString() + "px";
            return pixels;
        }

        private void UpdateWaterTankLevel(string measuredValue)
        {
            var newVolume = Tank.Radius * ((Tank.Height + 60) - float.Parse(measuredValue));
            using (WaterSensorDbContext context = DbContextFactory.CreateDbContext())
            {
                var newPercentage = (newVolume / Tank.Volume) * 100;
                var waterLevel = new WaterLevel { DateTimeMeasured = DateTime.Now, Id = Guid.NewGuid(), Percentage = newPercentage };
                context.Add(waterLevel);
                context.SaveChanges();
            }
        }

        private async Task UpdateBatteryLevel(string measuredValue)
        {
            using (WaterSensorDbContext context = DbContextFactory.CreateDbContext())
            {
                context.FireBeetleDevice.First().BatteryPercentage = measuredValue;
                await context.SaveChangesAsync();
            }
        }
    }
}
