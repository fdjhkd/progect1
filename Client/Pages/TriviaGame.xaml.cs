using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Newtonsoft.Json;



namespace Client
{
    public partial class TriviaGame : Window
    {
        public System.Diagnostics.Stopwatch watch;
        public string lastClickedAnswer;
        public JsonQuestionAnswer qa;
        public User currClient;

        private System.Windows.Threading.DispatcherTimer questionTimer;
        private const int QUESTION_DURATION_MS = 20000;

        public TriviaGame(User client)
        {
            currClient = new User(client);
            InitializeComponent();
            initializeQA();
        }

        public void initializeQA()
        {
            questionTimer?.Stop();

            JsonGlobalRequest request = new JsonGlobalRequest
            {
                type = "get_question"
            };

            qa = currClient.GetCommunicator().GetQuestionAnswer(
                JsonConvert.SerializeObject(request, new JsonSerializerSettings
                {
                    NullValueHandling = NullValueHandling.Ignore,
                    Formatting = Formatting.None
                }));
            if (qa.Data == null)
            {
                JsonGlobalRequest jsonRequest = new JsonGlobalRequest
                {
                    type = "get_game_results"
                };

                string gameResult = currClient.GetCommunicator().GetGameResults(
                    JsonConvert.SerializeObject(jsonRequest, new JsonSerializerSettings
                    {
                        NullValueHandling = NullValueHandling.Ignore,
                        Formatting = Formatting.None
                    }));

                GameScore gs = new GameScore(gameResult, currClient);
                gs.Show();
                Close();
                return;
            }

            // Показать вопрос и ответы
            QuestionBox.Text = qa.Data.Question;
            Answer.Items.Clear();
            foreach (KeyValuePair<string, string> entry in qa.Data.Answers)
            {
                Answer.Items.Add(entry.Value);
            }

            watch = System.Diagnostics.Stopwatch.StartNew();
            lastClickedAnswer = null;

            questionTimer = new System.Windows.Threading.DispatcherTimer();
            questionTimer.Interval = TimeSpan.FromMilliseconds(QUESTION_DURATION_MS);
            questionTimer.Tick += QuestionTimer_Tick;
            questionTimer.Start();
        }

        private void QuestionTimer_Tick(object sender, EventArgs e)
        {
            questionTimer.Stop();

            if (lastClickedAnswer == null)
            {
                Random rnd = new Random();
                int index = rnd.Next(qa.Data.Answers.Count);
                lastClickedAnswer = new List<string>(qa.Data.Answers.Values)[index];
            }

            SubmitAnswer();
            initializeQA();
        }

        private void Answer_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                if (Answer.SelectedIndex >= 0)
                {
                    lastClickedAnswer = Answer.Items[Answer.SelectedIndex].ToString();
                    questionTimer?.Stop();
                    SubmitAnswer();
                    initializeQA();
                }
            }
            catch
            {
            }
        }

        public void SubmitAnswer()
        {
            watch.Stop();

            if (qa?.Data?.Answers == null || lastClickedAnswer == null)
                return;

            foreach (KeyValuePair<string, string> entry in qa.Data.Answers)
            {
                if (entry.Value.Equals(lastClickedAnswer))
                {
                    JsonSubmitAnswerRequest jsonRequest = new JsonSubmitAnswerRequest
                    {
                        type = "submit_answer",
                        answer_id = int.Parse(entry.Key),
                        time_to_answer = watch.ElapsedMilliseconds
                    };

                    string resp = currClient.GetCommunicator().SubmitAnswerRequest(
                        JsonConvert.SerializeObject(jsonRequest, new JsonSerializerSettings
                        {
                            NullValueHandling = NullValueHandling.Ignore,
                            Formatting = Formatting.None
                        }));
                    break;
                }
            }
        }

        private void ExitGame_Click(object sender, RoutedEventArgs e)
        {
            JsonGlobalRequest jsonRequest = new JsonGlobalRequest
            {
                type = "back_to_menu"
            };

            string resp = currClient.GetCommunicator().BackToMenuRequest(
                JsonConvert.SerializeObject(jsonRequest, new JsonSerializerSettings
                {
                    NullValueHandling = NullValueHandling.Ignore,
                    Formatting = Formatting.None
                }));

            if (resp.Equals("back_to_menu: success"))
            {
                Main main = new Main(currClient);
                main.Show();
                Close();
            }
        }
    }
}
