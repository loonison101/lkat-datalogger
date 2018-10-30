﻿// <auto-generated />
using System;
using LKAT.Cmd;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Infrastructure;
using Microsoft.EntityFrameworkCore.Storage.ValueConversion;

namespace LKAT.Cmd.Migrations
{
    [DbContext(typeof(CsvDbContext))]
    partial class CsvDbContextModelSnapshot : ModelSnapshot
    {
        protected override void BuildModel(ModelBuilder modelBuilder)
        {
#pragma warning disable 612, 618
            modelBuilder
                .HasAnnotation("ProductVersion", "2.2.0-preview2-35157");

            modelBuilder.Entity("LKAT.Cmd.DbCsvRecord", b =>
                {
                    b.Property<int>("Id")
                        .ValueGeneratedOnAdd();

                    b.Property<float>("Age");

                    b.Property<float>("Altitude");

                    b.Property<float>("Hdop");

                    b.Property<float>("Latitude");

                    b.Property<float>("Longitude");

                    b.Property<string>("RawLine");

                    b.Property<int>("Satellites");

                    b.Property<float>("Speed");

                    b.Property<string>("Uuid");

                    b.Property<DateTime>("When");

                    b.HasKey("Id");

                    b.ToTable("CsvRecords");
                });
#pragma warning restore 612, 618
        }
    }
}
