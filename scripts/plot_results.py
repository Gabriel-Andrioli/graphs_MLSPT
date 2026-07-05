import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
import re

def render_table(data, filename, title, subtitle=None):
    fig, ax = plt.subplots(figsize=(20, len(data) * 0.35 + 2.0))
    ax.axis('off')
    
    if subtitle:
        ax.set_title(title, fontweight='bold', size=16, pad=35)
        ax.text(0.5, 1.02, subtitle, transform=ax.transAxes, ha='center', va='bottom', fontsize=12, style='italic', color='#555555')
    else:
        ax.set_title(title, fontweight='bold', size=16, pad=20)
    
    table = ax.table(cellText=data.values, 
                     colLabels=data.columns, 
                     cellLoc='center', 
                     loc='center')
    
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1, 1.5)
    
    for (row, col), cell in table.get_celld().items():
        if row == 0:
            cell.set_text_props(weight='bold', color='white')
            cell.set_facecolor('#4c72b0')
        else:
            cell.set_facecolor('#f5f5f5' if row % 2 == 0 else '#ffffff')

    plt.tight_layout()
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.close()

def get_project_root():
    current_path = os.path.abspath(os.getcwd())
    while not os.path.exists(os.path.join(current_path, "CMakeLists.txt")) and not os.path.exists(os.path.join(current_path, ".gitignore")):
        parent = os.path.dirname(current_path)
        if parent == current_path:
            return "."
        current_path = parent
    return current_path

def process_results():
    root_dir = get_project_root()
    out_dir = os.path.join(root_dir, "results")
    csv_file = os.path.join(out_dir, "results.csv")
            
    if not os.path.exists(csv_file):
        print(f"Erro: Arquivo results.csv nao encontrado em {csv_file}")
        return

    # Extrai o master_seed da primeira linha comentada
    master_seed = None
    with open(csv_file, 'r') as f:
        first_line = f.readline()
        if first_line.startswith('# master_seed:'):
            master_seed = first_line.split(':')[1].strip()

    print(f"Lendo {csv_file}...")
    df = pd.read_csv(csv_file, comment='#')
    
    # Extrai o N do nome da instancia (ex: scenario_n1000_l1000_...)
    def extract_n(instance_name):
        match = re.search(r'_n(\d+)_', instance_name)
        if match:
            return int(match.group(1))
        return 0
        
    df['N'] = df['instance'].apply(extract_n)

    # ---------------------------------------------------------
    # 1. Tabela com as medias (Apenas N <= 100)
    # ---------------------------------------------------------
    df_small = df[df['N'] <= 100].copy()
    
    if not df_small.empty:
        print("\n=== GERANDO TABELA DE MEDIAS (N <= 100) ===")
        # Colunas de algoritmo
        algo_cols = ['greedy', 'rand_0.3', 'rand_0.5', 'rand_0.8', 'react_0.5', 'react_1.0', 'react_2.0']
        time_cols = ['time_greedy', 'time_rand_0.3', 'time_rand_0.5', 'time_rand_0.8', 'time_react_0.5', 'time_react_1.0', 'time_react_2.0']
        all_cols = algo_cols + time_cols
        
        # Agrupa por instancia e calcula a media de custos e tempos
        df_mean = df_small.groupby(['instance', 'N'])[all_cols].mean().round(2).reset_index()
        if master_seed is not None:
            df_mean['master_seed'] = master_seed
        
        out_csv1 = os.path.join(out_dir, 'medias_n100.csv')
        df_mean.to_csv(out_csv1, index=False)
        print(f"Tabela salva em '{out_csv1}'")
        
        out_png1 = os.path.join(out_dir, 'tabela_medias_n100.png')
        subtitle = f"Semente Mestre: {master_seed}" if master_seed else None
        render_table(df_mean, out_png1, 'Medias dos Custos e Tempos (ms) - Instancias N <= 100', subtitle=subtitle)
        print(f"Imagem da tabela salva em '{out_png1}'")

        # ---------------------------------------------------------
        # 2. Boxplot de distribuicao por algoritmo
        # ---------------------------------------------------------
        print("\n=== GERANDO BOXPLOT GERAL ===")
        # Derrete (Melt) o dataframe para o seaborn
        df_melt = df_small.melt(id_vars=['instance', 'run_id', 'N'], 
                                value_vars=algo_cols,
                                var_name='Algorithm', 
                                value_name='Cost')
        
        plt.figure(figsize=(12, 6))
        sns.boxplot(data=df_melt, x='Algorithm', y='Cost', hue='N')
        plt.title('Distribuicao de Rotulos Usados (N <= 100)')
        plt.ylabel('Custo (Rotulos)')
        plt.xlabel('Algoritmo')
        plt.xticks(rotation=45)
        plt.tight_layout()
        out_box = os.path.join(out_dir, 'boxplot_geral.png')
        plt.savefig(out_box)
        print(f"Grafico salvo em '{out_box}'")

        # ---------------------------------------------------------
        # 2.b Histograma de distribuicao de custos
        # ---------------------------------------------------------
        print("\n=== GERANDO GRAFICO DE DENSIDADE (CURVAS) ===")
        plt.figure(figsize=(10, 6))
        sns.kdeplot(data=df_melt, x='Cost', hue='Algorithm', fill=True, alpha=0.2, linewidth=2, common_norm=False)
        plt.title('Densidade de Distribuicao dos Custos (N <= 100)')
        plt.ylabel('Densidade')
        plt.xlabel('Custo (Rotulos)')
        plt.tight_layout()
        out_hist = os.path.join(out_dir, 'histograma_geral.png')
        plt.savefig(out_hist)
        print(f"Grafico salvo em '{out_hist}'")
    else:
        print("\nNenhuma instancia com N <= 100 encontrada para gerar graficos.")

    # ---------------------------------------------------------
    # 3. Analise N >= 500 (Apenas Guloso)
    # ---------------------------------------------------------
    df_large = df[df['N'] >= 500].copy()
    if not df_large.empty:
        print("\n=== ANALISE INSTANCIAS GRANDES (N >= 500) ===")
        df_large_mean = df_large.groupby(['instance', 'N'])[['greedy', 'time_greedy']].mean().round(2).reset_index()
        if master_seed is not None:
            df_large_mean['master_seed'] = master_seed
        
        out_csv2 = os.path.join(out_dir, 'medias_n500.csv')
        df_large_mean.to_csv(out_csv2, index=False)
        print(f"Tabela salva em '{out_csv2}'")
        
        out_png2 = os.path.join(out_dir, 'tabela_medias_n500.png')
        subtitle = f"Semente Mestre: {master_seed}" if master_seed else None
        render_table(df_large_mean, out_png2, 'Custos e Tempos (ms) do Guloso - Instancias N >= 500', subtitle=subtitle)
        print(f"Imagem da tabela salva em '{out_png2}'")
        
        plt.figure(figsize=(10, 5))
        sns.barplot(data=df_large_mean, x='instance', y='greedy')
        plt.title('Custo do Guloso (N >= 500)')
        plt.ylabel('Custo (Rotulos)')
        plt.xlabel('Instancia')
        plt.xticks(rotation=90)
        plt.tight_layout()
        out_bar = os.path.join(out_dir, 'barplot_grandes.png')
        plt.savefig(out_bar)
        print(f"Grafico salvo em '{out_bar}'")

    print("\nProcessamento concluido com sucesso!")

if __name__ == "__main__":
    process_results()
