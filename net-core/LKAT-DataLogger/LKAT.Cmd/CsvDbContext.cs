using System;
using System.Linq;
using System.Reflection;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Query;
using Microsoft.EntityFrameworkCore.Query.Internal;
using Microsoft.EntityFrameworkCore.Storage;

namespace LKAT.Cmd
{
    
    public static class QueryableExtensions
    {
        private static readonly TypeInfo QueryCompilerTypeInfo = typeof(QueryCompiler).GetTypeInfo();

        private static readonly FieldInfo QueryCompilerField = typeof(EntityQueryProvider).GetTypeInfo().DeclaredFields.First(x => x.Name == "_queryCompiler");
        private static readonly FieldInfo QueryModelGeneratorField = typeof(QueryCompiler).GetTypeInfo().DeclaredFields.First(x => x.Name == "_queryModelGenerator");
        private static readonly FieldInfo DataBaseField = QueryCompilerTypeInfo.DeclaredFields.Single(x => x.Name == "_database");
        private static readonly PropertyInfo DatabaseDependenciesField = typeof(Database).GetTypeInfo().DeclaredProperties.Single(x => x.Name == "Dependencies");

        public static string ToSql<TEntity>(this IQueryable<TEntity> query)
        {
            var queryCompiler = (QueryCompiler) QueryCompilerField.GetValue(query.Provider);
            var queryModelGenerator = (QueryModelGenerator)QueryModelGeneratorField.GetValue(queryCompiler);
            var queryModel = queryModelGenerator.ParseQuery(query.Expression);
            var database = DataBaseField.GetValue(queryCompiler);
            var databaseDependencies = (DatabaseDependencies) DatabaseDependenciesField.GetValue(database);
            var queryCompilationContext = databaseDependencies.QueryCompilationContextFactory.Create(false);
            var modelVisitor = (RelationalQueryModelVisitor) queryCompilationContext.CreateQueryModelVisitor();
            modelVisitor.CreateQueryExecutor<TEntity>(queryModel);
            var sql = modelVisitor.Queries.First().ToString();

            return sql;
        }
    }
    public class CsvDbContext : DbContext
    {
        public DbSet<DbCsvRecord> CsvRecords { get; set; }

        protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
        {
            optionsBuilder.UseSqlite(@"Data Source=" + CONSTANTS.DB_PATH);
        }
        
//        protected override void OnModelCreating(ModelBuilder modelBuilder)
//        {
//            modelBuilder.Entity<DbCsvRecord>()
//                .HasIndex(b => b.Uuid);
//        }
    }
    
    public class DbCsvRecord
    {
        public int Id { get; set; }
        public int     Satellites { get; set; }
        public float Hdop { get; set; }
        public float Latitude { get; set; }
        public float Longitude { get; set; }
        public float Age { get; set; }
//        public DateTime WhenDate { get; set; }
//        public string WhenTime { get; set; }
        public DateTime When { get; set; }
        public float Altitude { get; set; }
        public string Uuid { get; set; }
        public float Speed { get; set; }
        public string RawLine { get; set; }
        public int BatteryVoltage { get; set; }
    }
}